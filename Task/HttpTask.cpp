#include "HttpTask.h"

HttpTask::HttpTask(Channel *channel):m_channel(channel),Task()
{
    m_handler=new HttpHandler(m_channel->getFd());
}

void HttpTask::run(void* arg)
{
    ConnectionManager* conn=static_cast<ConnectionManager*>(arg);
    bool flag=m_handler->parseData();
    //解析数据失败或客户端断开连接就回收资源
    if(!flag)
    {
        conn->getEvLoop()->addTask(m_channel, Type::DEL);
        return;
    }
    //解析失败断开连接
    flag=m_handler->parseRequest();
    if(!flag)
        conn->getEvLoop()->addTask(m_channel,Type::DEL);
}

HttpTask::~HttpTask()
{
    delete m_handler;
}
