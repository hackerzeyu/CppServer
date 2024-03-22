## CppServer
    本项目采用C++11编写，搭建了一个只支持Linux平台的高性能服务器。整体采用one thread one loop模型，主loop负责接收连接，唤醒阻塞的子线程，子loop负责处理读写和异常事件，采用Rector反应堆模式。
​    通过Server类启动服务器，ConnectionManager类实现连接管理，Task目录实现业务与框架分离，避免了ConnectionManager类的冗余，提高了程序的可扩展性，Protocol目录则书写各种协议的通信和数据格式(本项目采用了http的协议格式)。此外还包含日志，工具，配置文件等模块，配置文件采用ini格式，可通过修改ini配置文件，设定服务器和日志的启动模式。
    该服务器可同时处理成百上千连接，通过创建2000个线程同时发送http请求并接收响应，服务器可顺利完成，支持高QPS场景。