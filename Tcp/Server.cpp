#include "Server.h"

void Server::init()
{
    System sys;
    //初始化系统获取进程路径并生成core文件
    sys.init();

    IniFile ini("../config/server.ini");
    string ip=ini["server"]["ip"];
    int port=ini["server"]["port"];
    int threadsNum=ini["server"]["threads"];
    int logSize=ini["log"]["log_size"];
    int level=ini["log"]["log_level"];

    auto logger=Singleton<Log>::instance();
    //设置日志大小最大为1M
    logger->setMaxLen(logSize);
    logger->setLevel(static_cast<Log::Level>(level));
    //启动日志
    //Singleton<Log>::instance()->open(sys.get_root_path()+"/Log/server.log");
    logger->open("../Log/server.log");
    m_mainLoop=new EventLoop;
    m_mainLoop->set_id(std::this_thread::get_id());
    //创建线程池(8个线程)
    Singleton<ThreadPool>::instance()->init(m_mainLoop,threadsNum);
    //设置监听
    setListen(ip,port);
}

void Server::setListen(string ip,uint16_t port,int backlog)
{
    m_lfd=::socket(AF_INET,SOCK_STREAM,0);
    if(m_lfd<0)
    {
        log_fatal("socket create failed!");
        exit(1);
    }
    //设置收发缓冲区为1M
    set_send_buf(1024*1024);
    set_recv_buf(1024*1024);
    int opt=1;
    int ret=::setsockopt(m_lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(ret<0)
    {
        log_error("set port reuseable failed!");
        exit(1);
    }
    struct sockaddr_in serv_addr;
    memset(&serv_addr,0,sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port= htons(port);
    serv_addr.sin_addr.s_addr= inet_addr(ip.c_str());
    ret=::bind(m_lfd,reinterpret_cast<struct sockaddr*>(&serv_addr),sizeof(serv_addr));
    if(ret<0)
    {
        log_fatal("bind ip and port failed,errmsg=%s", strerror(errno));
        exit(1);
    }
    ret=::listen(m_lfd,backlog);
    if(ret<0)
    {
        log_fatal("server listen failed!");
        exit(1);
    }
}

void Server::run()
{
    Channel* channel=new Channel(m_lfd,Channel::EV_READ, acceptConn, nullptr, nullptr, this);
    m_mainLoop->addTask(channel,Type::ADD);
    log_info("server is running...");
    m_mainLoop->run();
}

void Server::set_non_blocking(int fd)
{
    int flag= fcntl(fd,F_GETFL);
    if(flag<0)
    {
        log_error("get flag failed!");
        exit(1);
    }
    flag |= O_NONBLOCK;
    int ret=fcntl(fd,F_SETFL,flag);
    if(ret<0)
    {
        log_error("set nonblocking failed!");
        exit(1);
    }
}

void Server::acceptConn(void *arg)
{
    auto server=static_cast<class Server*>(arg);
    int cfd= ::accept(server->m_lfd, nullptr, nullptr);
    log_debug("new connection:%d",cfd);
    //设置cfd未非阻塞
    server->set_non_blocking(cfd);
    auto evLoop=Singleton<ThreadPool>::instance()->get();
    new ConnectionManager(cfd, evLoop);
}

void Server::set_recv_buf(int buff_size)
{
    int ret= ::setsockopt(m_lfd,SOL_SOCKET,SO_RCVBUF,&buff_size,sizeof(buff_size));
    if(ret<0)
        log_error("set recvbuf failed!");
}

void Server::set_send_buf(int buff_size)
{
    int ret= ::setsockopt(m_lfd,SOL_SOCKET,SO_SNDBUF,&buff_size,sizeof(buff_size));
    if(ret<0)
        log_error("set sendbuf failed!");
}



