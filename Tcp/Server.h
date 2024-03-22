#pragma once
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
using std::string;
#include <unistd.h>
#include "log.h"
#include "ThreadPool.h"
#include "ConnectionManager.h"
#include "System.h"
#include "ini_file.h"

class Server
{
    SINGLETON(Server);
public:
    void init();
    void setListen(string ip,uint16_t port,int backlog=128);
    void run();
    void set_non_blocking(int fd);                    //设置非阻塞
    void set_recv_buf(int buff_size);                 //设置接收缓冲区大小
    void set_send_buf(int buff_size);                 //设置发送缓冲区大小
    int getLfd() const{return m_lfd;}
private:
    static void acceptConn(void *arg);                //监听回调
private:
    int m_lfd;
    string m_ip;
    uint16_t m_port;
    EventLoop* m_mainLoop;
};