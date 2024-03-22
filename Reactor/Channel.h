#pragma once
#include <functional>

class Channel
{
public:
    enum Event
    {
        EV_READ=0x01,
        EV_WRITE=0x02,
        EV_TIMEOUT=0x04
    };
    using Handle=std::function<void(void *arg)>;
public:
    Channel(int fd,Event events,Handle readCallback,Handle writeCallback,Handle destroyCallback,void *arg);
    ~Channel();
    void writeEventEnable(bool flag);
    bool isWriteEventEnable();
    int getFd() const{return m_fd;}
    int getEv() const{return m_events;}
    void* getArg() const{return m_arg;}
    void setReadCallback(Handle readCallback){m_readCallback=readCallback;}
    void setWriteCallback(Handle writeCallback){m_writeCallback=writeCallback;}
public:
    Handle m_readCallback;      //读回调
    Handle m_writeCallback;     //写回调
    Handle m_destroyCallback;   //销毁回调
private:
    int m_fd;               //文件描述符
    int m_events;           //事件
    void* m_arg;            //参数
};

