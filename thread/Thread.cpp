#include "Thread.h"

void Thread::start()
{
    m_thread=std::thread(thread_func, this);
    m_id=m_thread.get_id();
    //线程分离
    m_thread.detach();
}

void Thread::stop()
{
    //必须由子线程自己主动退出,主线程没有权限杀死子线程
    if(std::this_thread::get_id()==m_id)
        pthread_exit(nullptr);
    else
        log_error("no privilege to exit child thread!");
}

void Thread::thread_func(void *arg)
{
    auto thread=static_cast<Thread*>(arg);
    thread->run();
}



