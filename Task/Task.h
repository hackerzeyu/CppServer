#pragma once

class Task
{
public:
    Task()=default;
    Task(void* arg):m_arg(arg){}
    ~Task()=default;
    void setArg(void* arg){m_arg=arg;}
    void* getArg(void* arg){return m_arg;}
    virtual void run(void* arg)=0;
private:
    void * m_arg;   //任务参数
};