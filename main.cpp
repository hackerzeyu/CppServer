#include "Tcp/Server.h"
using namespace std;

int main()
{
    auto server=Singleton<Server>::instance();
    server->init();
    server->run();
}