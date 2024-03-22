#include "EventPoller.h"

EventPoller::EventPoller(EventLoop* evLoop):m_evLoop(evLoop)
{
    create(m_maxConn);
}

EventPoller::~EventPoller()
{
    if(m_epfd>0)
    {
        ::close(m_epfd);
        m_epfd=0;
    }
    if(m_events)
    {
        delete[] m_events;
        m_events= nullptr;
    }
}

void EventPoller::create(int conn)
{
    m_maxConn=conn;
    m_epfd=::epoll_create(1);
    if(m_epfd<0)
    {
        log_fatal("epoll_create failed!");
        exit(1);
    }
    m_events=new epoll_event[conn];
}

int EventPoller::add()
{
    return epollCtl(EPOLL_CTL_ADD);
}
int EventPoller::del()
{
   int ret=epollCtl(EPOLL_CTL_DEL);
   //防止内存泄漏
   m_channel->m_destroyCallback(m_channel->getArg());
   return ret;
}
int EventPoller::mod()
{
    return epollCtl(EPOLL_CTL_MOD);
}

int EventPoller::epollCtl(int opt)
{
    struct epoll_event ev;
    int fd=m_channel->getFd();
    ev.data.fd=fd;
    int events=0;
    if(m_channel->getEv() & Channel::EV_READ)
        events |= EPOLLIN;
    if(m_channel->getEv() & Channel::EV_WRITE)
        events|=EPOLLOUT;
    ev.events=events;
    return ::epoll_ctl(m_epfd,opt,fd,&ev);
}


void EventPoller::run(int milliseconds)
{
    int num= ::epoll_wait(m_epfd,m_events,m_maxConn,milliseconds);
    if(num<0)
    {
        log_error("epoll_wait failed,errmsg=%s", strerror(errno));
    }
    else if(num==0)
    {
        //超时
    }
    else
    {
        for (int i = 0; i < num; i++)
        {
            int ev = m_events[i].events;
            int fd = m_events[i].data.fd;
            if (ev & EPOLLHUP)
            {
                //此处应该触发事件异常回调，为了省略触发读回调直接断开连接
                log_error("socket hang up,connfd=%d", fd);
                m_evLoop->addTask(m_channel,Type::DEL);
                continue;
            }
            if(ev & EPOLLERR)
            {
                log_error("socket error,connfd=%d",fd);
                m_evLoop->addTask(m_channel,Type::DEL);
                continue;
            }
            if (ev & EPOLLIN)
            {
                //触发读回调
                m_evLoop->activate(fd,EPOLLIN);
            }
            if(ev & EPOLLOUT)
            {
                //触发写回调
                m_evLoop->activate(fd,EPOLLOUT);
            }
        }
    }
}