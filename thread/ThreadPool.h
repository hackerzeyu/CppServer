#include <vector>
#include "WorkerThread.h"
#include "Singleton.h"

class ThreadPool
{
    SINGLETON(ThreadPool);
public:
    //创建线程池并启动
    void init(EventLoop* mainLoop,int num);
    //获取子线程实例
    EventLoop* get();
private:
    int m_threadNum;
    std::vector<WorkerThread*> m_pool;              //线程队列
    int m_index=0;
    EventLoop* m_mainLoop;
};
