#include "HttpHandler.h"

HttpHandler::HttpHandler(int cfd):m_cfd(cfd),m_requestMsg(""),m_method(""),m_url(""),m_version(""),
    m_reqHeaderNum(0)
{
    m_reqHeaders.clear();
}

void HttpHandler::recvData()
{
    std::ostringstream oss;
    char buf[1024]="";
    while (true)
    {
        memset(buf,0,sizeof(buf));
        int len=::recv(m_cfd,buf,sizeof(buf),0);
        if(len<0)
        {
            if(errno==EAGAIN || errno==EWOULDBLOCK)
            {
                //数据读完
                break;
            }
            if(errno==EINTR)
            {
                log_debug("interrupted by signal");
                continue;
            }
            log_error("recv data failed,errmsg=%s", strerror(errno));
            m_requestMsg.clear();
            return;
        }
        else if(len==0)
        {
            //客户端断开连接,清空请求数据
            log_debug("client close");
            m_requestMsg.clear();
            return;
        }
        else
        {
            oss<<buf;
        }
        usleep(10);
    }
    m_requestMsg=oss.str();
    log_debug("request:%s",m_requestMsg.c_str());
}

bool HttpHandler::parseData()
{
    //读取数据
    recvData();
    if(m_requestMsg.empty())
    {
        return false;
    }
    //获取请求行
    size_t pos=m_requestMsg.find_first_of("\r\n");
    string requestLine=m_requestMsg.substr(0,pos);
    std::istringstream iss(m_requestMsg);
    iss>>m_method>>m_url>>m_version;
    log_debug("method=%s,url=%s,version=%s",m_method.c_str(),m_url.c_str(),m_version.c_str());
    //只处理get请求
    if(strcasecmp(m_method.c_str(),"get")!=0)
        return false;
    //获取请求头键值对
    size_t first=pos+2,last=0;
    while (true)
    {
        /*
         * Host: 1\r\n
         * User-Agent: 2\r\n
         * \r\n
         * */
        last=m_requestMsg.find_first_of("\r\n",first);
        string line=m_requestMsg.substr(first,last-first);
        if(line.empty())
            break;
        pos=line.find_first_of(":");
        string key=line.substr(0,pos);
        string value=line.substr(pos+2,last-pos-2);
        m_reqHeaders.insert(std::make_pair(key,value));
        m_reqHeaderNum++;
        first=last+2;
    }
//    for(auto it=m_reqHeaders.begin();it!=m_reqHeaders.end();it++)
//    {
//        std::cout<<it->first<<":"<<it->second<<std::endl;
//    }
    return true;
}

bool HttpHandler::parseRequest()
{
    ::decodeMsg(m_url.data(),m_url.data());
    string file="";
    if(m_url=="/")
        file="./";
    else
        file=m_url.substr(1);
    //获取文件属性
    struct stat st;
    int ret= stat(file.c_str(),&st);
    if(ret<0)
    {
        if(sendResponse(404,"Not Found",::getFileType(".html"),-1)==-1)
            return false;
        if(sendFile("./error.html",-1)==-1)
            return false;
    }
    //目录
    else if(S_ISDIR(st.st_mode))
    {
        if(sendResponse(200,"OK",::getFileType(".html"),-1)==-1)
            return false;
        if(sendDir(file.c_str())==-1)
            return false;
    }
    else if(S_ISREG(st.st_mode))
    {
        if(sendResponse(200,"OK",::getFileType(file.c_str()),st.st_size)==-1)
            return false;
        if(sendFile(file.c_str(),st.st_size)==-1)
            return false;
    }
    return true;
}

int HttpHandler::sendResponse(int code,const char* msg,const char* type,int length)
{
    char buf[4096] = {0};
    sprintf(buf, "HTTP/1.1 %d %s\r\n", code, msg);
    // 响应头
    sprintf(buf + strlen(buf), "Content-Type: %s\r\n", type);
    sprintf(buf + strlen(buf), "Content-Length: %d\r\n\r\n", length);
    return sendBuf(buf, strlen(buf));
}

int HttpHandler::sendBuf(char* buf,int len)
{
    char* pt=buf;
    int count=0,size=len;
    while(size>0)
    {
        int count = ::send(m_cfd, pt, size, MSG_NOSIGNAL);
        if (count < 0)
        {
            if (errno == EAGAIN || errno==EWOULDBLOCK)
            {
                //写缓冲区已满
                continue;
            }
            else if(errno==EINTR)
            {
                log_debug("interrupted by signal");
                continue;
            }
            log_debug("send failed,errmsg=%s", strerror(errno));
            return -1;
        }
        else if (count == 0)
        {
            log_debug("client closed");
            return -1;
        }
        else
        {
            size-=count;
            pt+=count;
        }
        usleep(10);
    }
    return len;
}

int HttpHandler::sendFile(const char* file,int size)
{
    int fd=open(file,O_RDONLY);
    if(fd<0)
    {
        log_error("open %s failed,errmsg=%s",file, strerror(errno));
        return -1;
    }
    //获取404.html大小
    if(size<0)
    {
        struct stat st;
        stat("error.html",&st);
        size=st.st_size;
    }
    //一次发4k
    char buf[4096]={0};
    int len=0;
    int sum=0;
    while (sum<size)
    {
        memset(buf,0,sizeof(buf));
        len=::read(fd,buf,sizeof(buf));
        if (len<0)
        {
            log_error("read failed,errmsg=%s", strerror(errno));
            return -1;
        }
        else if(len==0)
        {
            break;
        }
        else
        {
            if(sendBuf(buf,len)==-1)
                return -1;
            sum+=len;
        }
    }
    return size;
}

int HttpHandler::sendDir(const char* dirName)
{
    std::ostringstream oss;
    char buf[2048]="";
    sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName);
    oss<<buf;
    struct dirent** namelist;
    int num = scandir(dirName, &namelist, NULL, alphasort);
    for (int i = 0; i < num; ++i)
    {
        memset(buf,0,sizeof(buf));
        // 取出文件名 namelist 指向的是一个指针数组 struct dirent* tmp[]
        char* name = namelist[i]->d_name;
        ::decodeMsg(name,name);
        struct stat st;
        char subPath[1024] = { 0 };
        sprintf(subPath, "%s/%s", dirName, name);
        stat(subPath, &st);
        if (S_ISDIR(st.st_mode))
        {
            // a标签 <a href="">name</a>
            sprintf(buf,
                    "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>",
                    name, name, st.st_size);
        }
        else
        {
            sprintf(buf,
                    "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
                    name, name, st.st_size);
        }
        oss<<buf;
        delete namelist[i];
    }
    sprintf(buf, "</table></body></html>");
    oss<<buf;
    delete namelist;
    if(sendBuf(oss.str().data(),oss.str().length())<0)
    {
        return -1;
    }
    return oss.str().length();
}

