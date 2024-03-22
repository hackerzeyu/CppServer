#include "ThreadPool.h"

void ThreadPool::init(EventLoop* mainLoop,int num)
{
    m_mainLoop=mainLoop;
    m_threadNum=num;
    for(int i=0;i<num;i++)
    {
        auto thread=new WorkerThread();
        m_pool.push_back(thread);
        thread->start();
        log_info("create a thread:%x",thread);
    }
}

EventLoop* ThreadPool::get()
{
    EventLoop* evLoop=m_mainLoop;
    if(m_threadNum>0)
    {
        int index=m_index;
        evLoop=m_pool[index]->get();
        m_index=(m_index+1)%m_threadNum;
    }
    return evLoop;
}





