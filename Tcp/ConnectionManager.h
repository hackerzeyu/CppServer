#pragma once
#include "EventLoop.h"
#include "HttpTask.h"

class HttpTask;
class ConnectionManager
{
public:
    ConnectionManager()=delete;
    ConnectionManager(int cfd,EventLoop* evLoop);
    ~ConnectionManager();
    EventLoop* getEvLoop(){return m_evLoop;}
    //销毁回调
    static void destroy(void *arg);
private:
    Channel* m_channel;
    EventLoop* m_evLoop;
    HttpTask* m_httpTask;
};