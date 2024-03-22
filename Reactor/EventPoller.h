#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include "Channel.h"
#include "log.h"
#include "EventLoop.h"
#define MAX_CONN 1024
class EventLoop;
class EventPoller
{
public:
    EventPoller(EventLoop* evLoop);
    ~EventPoller();
    void create(int conn);
    int add();
    int del();
    int mod();
    int epollCtl(int opt);
    void run(int milliseconds=10);    //运行
    inline void setChannel(Channel* channel){m_channel=channel;}
private:
    int m_epfd;
    int m_maxConn=MAX_CONN;             //一次最大连接数
    struct epoll_event* m_events;
    Channel* m_channel;                 //fd通道
    EventLoop* m_evLoop;
};
