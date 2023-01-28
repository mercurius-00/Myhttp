//
// Created by Administrator on 2023/1/28.
//

#ifndef HTTPD_DEMO_THREAD_H
#define HTTPD_DEMO_THREAD_H

#endif //HTTPD_DEMO_THREAD_H

#include <winsock2.h>
using namespace std;

//从套接字读取一行(返回读取数据字节数)
int get_line(int sock, char *buff, int size);

//处理请求线程函数
DWORD WINAPI accept_request(LPVOID arg);