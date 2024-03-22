#pragma once
#include <cstring>
#include <dirent.h>
#include <map>
#include <string>
#include <sstream>
using std::string;
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "log.h"
#include "convert.h"

//处理HTTP请求
class HttpHandler
{
public:
    HttpHandler()=delete;
    HttpHandler(int cfd);
    bool parseData();                                 //解析数据
    void recvData();                                  //接收数据
    bool parseRequest();                              //解析请求行
    //发送响应
    int sendResponse(int code,const char* msg,const char* type,int length);
    int sendBuf(char* buf,int len);                   //发送数据
    int sendFile(const char* file,int size);          //发送文件
    int sendDir(const char* file);                    //发送目录
private:
    int m_cfd;                              //客户端套接字
    string m_requestMsg;                    //请求数据
    string m_method;                        //方式
    string m_url;                           //网址
    string m_version;                       //版本号
    int m_reqHeaderNum;                     //请求头键值对数
    std::map<string,string> m_reqHeaders;   //请求头
};

