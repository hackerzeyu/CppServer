#include "System.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/resource.h>

void System::init()
{
    m_root_path=get_root_path();
    core_dump();
    chdir("../web-http");
}

string System::get_root_path()
{
    if(!m_root_path.empty())
        return m_root_path;
    char path[1024]={0};
    int len= readlink("/proc/self/exe",path,sizeof(path));
    if(len<0 || len>=1024)
        return "";
    for(int i=len;i>=0;i--)
    {
        if(path[i]=='/')
        {
            path[i]='\0';
            break;
        }
    }
    return path;
}

void System::core_dump()
{
    struct rlimit x;
    x.rlim_cur = RLIM_INFINITY;
    x.rlim_max = RLIM_INFINITY;
    setrlimit(RLIMIT_CORE, &x);
}