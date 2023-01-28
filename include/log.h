//
// Created by Administrator on 2023/1/28.
//

#ifndef HTTPD_DEMO_LOG_H
#define HTTPD_DEMO_LOG_H

#endif //HTTPD_DEMO_LOG_H

#include <iostream>
#include <string.h>
#include <winsock2.h>
using namespace std;

void print_color(string str, char color);
void print_color(int str, char color);

//错误处理
void error_die(const char *str);

//取消实现
void unimplement(SOCKET client_socket);

//资源不存在处理
void resource_not_found(SOCKET client_socket, const char *file_name);