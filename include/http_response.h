//
// Created by Administrator on 2023/1/28.
//

#ifndef HTTPD_DEMO_HTTP_RESPONSE_H
#define HTTPD_DEMO_HTTP_RESPONSE_H

#endif //HTTPD_DEMO_HTTP_RESPONSE_H

#include <winsock2.h>
#include <fstream>
using namespace std;

const char *get_content_type(const char *file_name);

//发送响应包头信息
void send_header(SOCKET client_socket, const char *resource_type);

//发送响应包资源内容
void send_content(SOCKET client_socket, ifstream *file, bool file_is_binary);

//发送请求的资源文件(包含send_header()与send_content())
void send_server_file(SOCKET client_socket, const char *file_name);