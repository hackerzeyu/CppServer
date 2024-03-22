#include "Channel.h"

Channel::Channel(int fd,Event events,Handle readCallback,Handle writeCallback,Handle destroyCallback,void *arg):m_fd(fd),
    m_events((int)events),m_readCallback(readCallback),m_writeCallback(writeCallback),m_destroyCallback(destroyCallback),m_arg(arg)
{
}

void Channel::writeEventEnable(bool flag)
{
    if(flag)
        m_events|=static_cast<int>(EV_WRITE);
    else
        m_events &= ~static_cast<int>(EV_WRITE);
}

bool Channel::isWriteEventEnable()
{
    return m_events & static_cast<int>(EV_WRITE);
}

Channel::~Channel()
{
}
