#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <errno.h>

const char* msg="GET /error.html HTTP/1.1\r\n"
                "Host: 101.132.163.255:8080\r\n"
                "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/122.0.0.0 Safari/537.36 Edg/122.0.0.0\r\n"
                "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
                "Accept-Encoding: gzip, deflate\r\n"
                "Accept-Language: zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\n"
                "Cache-Control: max-age=0\r\n"
                "Upgrade-Insecure-Requests: 1\r\n\r\n";
void sendBuf()
{
    int fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr("101.132.163.255");
    serv_addr.sin_port=htons(8080);
    connect(fd,(struct sockaddr*)(&serv_addr),sizeof(serv_addr));
    int len= strlen(msg);
    char* buf=new char[len];
    memcpy(buf,msg,len);
    send(fd,buf,len,0);
    delete[] buf;
    char tmp[1024]="";
    while(true)
    {
	    memset(tmp,0,sizeof(tmp));
	    int ret=recv(fd,tmp,sizeof(tmp),0);
	    if(ret<0 && errno==EAGAIN)
		    break;
    }
}

int main(int argc,char** argv)
{
    if(argc<2)
	    return 0;

    std::vector<std::thread> threadsVec;
    int threads=atoi(argv[1]);
    for(int i=0;i<threads;i++)
    {
	    std::thread t(sendBuf);
	    threadsVec.push_back(std::move(t));
    }
    for(int i=0;i<threads;i++)
    {
	    threadsVec[i].join();
    }
    return 0;
}
