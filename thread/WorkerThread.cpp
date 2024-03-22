#include "WorkerThread.h"

WorkerThread::WorkerThread():Thread()
{
    m_evLoop=new EventLoop();
}

void WorkerThread::run()
{
    //线程启动时设置线程ID
    m_evLoop->set_id(std::this_thread::get_id());
    m_evLoop->run();
}