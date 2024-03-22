cmake_minimum_required(VERSION 3.16)
project(WebServer)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${PROJECT_SOURCE_DIR}/Log
        ${PROJECT_SOURCE_DIR}/Reactor
        ${PROJECT_SOURCE_DIR}/Tcp
        ${PROJECT_SOURCE_DIR}/thread
        ${PROJECT_SOURCE_DIR}/utility
        ${PROJECT_SOURCE_DIR}/Protocol
        ${PROJECT_SOURCE_DIR}/Task
)

file(GLOB_RECURSE SRC "utility/*.cpp" "thread/*.cpp" "Log/*.cpp" "Reactor/*.cpp" "Tcp/*.cpp" "Task/*.cpp" "Protocol/*.cpp")

add_executable(WebServer main.cpp ${SRC})
target_link_libraries(WebServer pthread)