#pragma once
#include <sys/socket.h>
#include <sys/types.h>
#include <cstring>
#include "Task.h"
#include "log.h"
#include "ConnectionManager.h"

class ConnectionManager;
class ReadTask:public Task
{
public:
    ReadTask()=delete;
    ReadTask(Channel* channel);
    int readn(char* buf,int size);
    void run(void* arg) override;
private:
    Channel* m_channel;
};