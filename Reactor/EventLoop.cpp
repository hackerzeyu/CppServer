#include "EventLoop.h"

EventLoop::EventLoop()
{
    //创建epoll实例
    m_epoll = new EventPoller(this);
    //创建socketpair
    int ret = ::socketpair(AF_UNIX, SOCK_STREAM, 0, m_socketPair);
    if (ret < 0)
    {
        log_error("create socketpair failed!");
        exit(1);
    }
    //将唤醒fd添加到任务队列
    Channel* channel=new Channel(m_socketPair[1],Channel::EV_READ,recvMsg, nullptr, nullptr, this);
    addTask(channel,Type::ADD);
}

void EventLoop::addTask(Channel* channel,Type type)
{
    std::unique_lock<std::mutex> lck(m_mutex);
    ChannelTask* task=new ChannelTask;
    task->channel=channel;
    task->type=type;
    m_taskQueue.push(task);
    lck.unlock();
    if(std::this_thread::get_id()!=m_id)
    {
        //若操作任务队列的是主线程,则需要唤醒正在阻塞的子线程
        wake();
    }
    else
    {
        //子线程处理任务队列
        processQueue();
    }
}

void EventLoop::processQueue()
{
    while (!m_taskQueue.empty())
    {
        std::unique_lock<std::mutex> lck(m_mutex);
        ChannelTask* task=m_taskQueue.front();
        m_taskQueue.pop();
        lck.unlock();
        Type type=task->type;
        Channel* channel=task->channel;
        m_epoll->setChannel(channel);
        switch (type) {
            case Type::ADD:
                add(channel);
                break;
            case Type::DEL:
                del(channel);
                break;
            case Type::MOD:
                mod(channel);
                break;
        }
        //处理完销毁任务
        delete task;
    }
}

void EventLoop::run()
{
    while (true)
    {
        m_epoll->run();
        processQueue();
    }
}

void EventLoop::add(Channel* channel)
{
    if(m_channelMap.find(channel->getFd())==m_channelMap.end())
    {
        m_channelMap[channel->getFd()] = channel;
        m_epoll->setChannel(channel);
        m_epoll->add();
    }
}

void EventLoop::del(Channel* channel)
{
    auto it=m_channelMap.find(channel->getFd());
    if(it!=m_channelMap.end())
    {
        m_epoll->setChannel(channel);
        m_epoll->del();
    }
}

void EventLoop::mod(Channel* channel)
{
    auto it=m_channelMap.find(channel->getFd());
    if(it!=m_channelMap.end())
    {
        m_epoll->setChannel(channel);
        m_epoll->mod();
    }
}

void EventLoop::eraseChannel(Channel* channel)
{
    auto it=m_channelMap.find(channel->getFd());
    if(it!=m_channelMap.end())
    {
        m_channelMap.erase(it);
        ::close(channel->getFd());
        delete channel;
    }
}

void EventLoop::activate(int fd,int events)
{
    Channel* channel=m_channelMap[fd];
    if(events & EPOLLIN)
    {
        //触发读回调
        channel->m_readCallback(channel->getArg());
    }
    if(events & EPOLLOUT)
    {
        //触发写回调
        channel->m_writeCallback(channel->getArg());
    }
}

void EventLoop::recvMsg(void *arg)
{
    auto evLoop=static_cast<EventLoop*>(arg);
    char msg[10]={0};
    ::recv(evLoop->m_socketPair[1],msg,sizeof msg,0);
}

void EventLoop::wake()
{
    const char* msg="hello";
    send(m_socketPair[0],msg,strlen(msg)+1,0);
}


