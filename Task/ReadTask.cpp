#include "ReadTask.h"

ReadTask::ReadTask(Channel* channel):Task(),m_channel(channel)
{
}

int ReadTask::readn(char* buf, int size)
{
    char* pt=buf;
    int count=size;
    while (count>0)
    {
        int len=::recv(m_channel->getFd(),pt,count,0);
        if(len<0)
        {
            if(errno==EAGAIN || errno==EWOULDBLOCK)
            {
                continue;
            }
            else if(errno==EINTR)
            {
                continue;
            }
            return -1;
        }
        else if(len==0)
        {
            return size-count;
        }
        else if(len>0)
        {
            count-=len;
            pt+=len;
        }
    }
    return size;
}

void ReadTask::run(void* arg)
{
    log_debug("read task start working...");
    ConnectionManager *conn = static_cast<ConnectionManager *>(arg);
    int len = 0;
    int ret = readn((char *) &len, 4);
    //读取异常
    if (ret < 0)
    {
        log_error("read len failed,error msg=%s", strerror(errno));
        conn->getEvLoop()->addTask(m_channel,Type::DEL);
        return;
    }
    //客户端断开连接
    else if(ret<4)
    {
        log_debug("client closed,connfd=%d",m_channel->getFd());
        //断开连接
        conn->getEvLoop()->addTask(m_channel, Type::DEL);
        log_debug("read task end...");
        return;
    }
    log_debug("thread id:%ld,recv msg len:%d", conn->getEvLoop()->get_id(), len);
    char *buf = new char[len + 1];
    ret=readn(buf, len);
    //读取异常
    if(ret<0)
    {
        log_error("read failed,error msg=%s", strerror(errno));
        //断开连接
        goto free;
    }
    else if(ret<len)
    {
        log_debug("client closed,connfd=%d",m_channel->getFd());
        //断开连接
        goto free;
    }
    buf[len] = '\0';
    log_debug("thread id:%ld,recv msg:%s", conn->getEvLoop()->get_id(), buf);
    log_debug("read task end...");
    free:
    conn->getEvLoop()->addTask(m_channel,Type::DEL);
    delete[] buf;
}