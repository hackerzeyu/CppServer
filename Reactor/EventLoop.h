#pragma once
#include <cstring>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include "EventPoller.h"

enum class Type
{
    ADD,
    DEL,
    MOD
};

//fd任务
struct ChannelTask
{
    Type type;
    Channel* channel;
};

class EventPoller;
class EventLoop
{
public:
    EventLoop();
    void addTask(Channel* channel,Type type);//添加任务
    void processQueue();                     //处理任务队列
    void add(Channel* channel);
    void del(Channel* channel);
    void mod(Channel* channel);
    void activate(int fd,int events);       //激活事件回调
    void run();                             //循环监听
    void eraseChannel(Channel* channel);    //删除channel
    inline void set_id(std::thread::id id){m_id=id;}
    inline std::thread::id get_id(){return m_id;}
private:
    static void recvMsg(void *arg);         //唤醒读回调
    void wake();                            //唤醒线程函数
private:
    std::thread::id m_id;
    int m_socketPair[2];
    EventPoller* m_epoll;
    std::map<int,Channel*> m_channelMap;   //fd映射
    std::queue<ChannelTask*> m_taskQueue;   //任务队列
    std::mutex m_mutex;
};

