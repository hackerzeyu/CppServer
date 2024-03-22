#pragma once
#include <mutex>
#include <condition_variable>
#include "Thread.h"
#include "../Reactor/EventLoop.h"

class WorkerThread:public Thread
{
public:
    WorkerThread();
    ~WorkerThread()=default;
    void run() override;
    EventLoop* get(){return m_evLoop;}
private:
    std::mutex m_mutex;
    EventLoop* m_evLoop;
};