#include "ConnectionManager.h"

ConnectionManager::ConnectionManager(int cfd,EventLoop* evLoop):m_evLoop(evLoop)
{
    m_channel=new Channel(cfd,Channel::EV_READ, nullptr, nullptr, destroy,this);
    m_httpTask=new HttpTask(m_channel);
    auto http=std::bind(&HttpTask::run,m_httpTask, this);
    m_channel->setReadCallback(http);
    //添加任务
    m_evLoop->addTask(m_channel,Type::ADD);
}

void ConnectionManager::destroy(void *arg)
{
    ConnectionManager* conn=static_cast<ConnectionManager*>(arg);
    delete conn;
}

ConnectionManager::~ConnectionManager()
{
    int connfd=m_channel->getFd();
    m_evLoop->eraseChannel(m_channel);
    delete m_httpTask;
    log_debug("resource successfully freed,connfd=%d",connfd);
}