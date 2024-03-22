#pragma once
#include <string>
using std::string;
class System
{
public:
    System()=default;
    ~System()=default;
    void init();
    string get_root_path();
private:
    //生成core文件
    void core_dump();
private:
    string m_root_path;
};