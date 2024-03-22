#pragma once
#include <thread>
#include "log.h"

class Thread
{
public:
    Thread():m_id(){}
    virtual ~Thread()=default;
    //启动线程
    void start();
    //线程运行函数
    virtual void run()=0;
    //线程终止函数
    void stop();
private:
    static void thread_func(void *arg);
protected:
    std::thread m_thread;
    std::thread::id m_id;
};