#pragma once
#include "HttpHandler.h"
#include "ConnectionManager.h"
#include "Task.h"

class ConnectionManager;
class HttpTask:public Task
{
public:
    HttpTask()=delete;
    HttpTask(Channel* channel);
    ~HttpTask();
    void run(void* arg) override;
private:
    HttpHandler* m_handler;
    Channel* m_channel;
};

