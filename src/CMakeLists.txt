cmake_minimum_required(VERSION 3.24)
project(httpd_demo)

set(CMAKE_CXX_STANDARD 17)
link_libraries(ws2_32 wsock32)

add_executable(httpd_demo main.cpp http/http_connect.cpp log/log.cpp ../include/log.h http/http_response.cpp ../include/http_response.h ../include/http_connect.h thread/thread.cpp ../include/thread.h)


