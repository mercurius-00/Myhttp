//
// Created by Administrator on 2023/1/28.
//

#include <winsock2.h>
#include <iostream>
#include <log.h>
#include <sys/stat.h>
#include "http_response.h"

using namespace std;

//从套接字读取一行(返回读取数据字节数)
int get_line(int sock, char *buff, int size){
    char charRecv = 0;
    int i = 0;
    while(charRecv!='\n' && i<size-1){
        if(recv(sock, &charRecv, 1, 0) > 0){
            if(charRecv=='\r'){
                if(recv(sock, &charRecv, 1, MSG_PEEK)>0 && charRecv=='\n') recv(sock, &charRecv, 1, 0);
                else charRecv = '\n';
            }
            buff[i++] = charRecv;
        }
        else break;
    }
    buff[i] = 0;
    return i;
}

//处理请求线程函数
DWORD WINAPI accept_request(LPVOID arg){
//解析套接字请求
char resources_path[100] = "resources";  //资源目录名
char head[1024];
char buff[1024];    //存一行请求报文
char mode[200]; //存请求方法名
char resource[200]; //存请求URL
char protocol[200];   //存请求协议版本
int head_pointer = 0, temp_pointer = 0;
int client_sock = (SOCKET)arg;  //传入参数
int chars_count;    //报文每行字符数

//读取首行并读完剩余行
chars_count = get_line(client_sock, head, sizeof(head));
while(chars_count > 0 && strcmp(buff, "\n")){
chars_count = get_line(client_sock, buff, sizeof(buff));
}
cout<<"In thread "<<GetCurrentThreadId()<<":\nrequest: "<<head;

//解析方法名
while(!isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) mode[temp_pointer++] = head[head_pointer++];
mode[temp_pointer] = 0;
temp_pointer = 0;
while(isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) head_pointer++;

//解析URL
while(!isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) resource[temp_pointer++] = head[head_pointer++];
resource[temp_pointer] = 0;
temp_pointer = 0;
while(isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) head_pointer++;

//解析协议版本
while(!isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) protocol[temp_pointer++] = head[head_pointer++];
protocol[temp_pointer] = 0;
temp_pointer = 0;
while(isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) head_pointer++;

//stricom()用于不区分大小写比较字符串，返回字符串差值(相同则0)，若方法名合法则取消实现
if(stricmp(mode, "GET") && stricmp(mode, "POST")){
unimplement(client_sock);
return 1;
}

//获取资源完整路径
if(resource[strlen(resource)-1] == '/') strcat(resources_path, strcat(resource, "index.html"));
else strcat(resources_path, resource);

//判断resources_path是否为目录
struct stat file_path_status;   //用于存储resources_path文件状态
if(stat(resources_path, &file_path_status) == -1){  //若获取文件状态失败，返回-1
cout<<124142<<endl;
resource_not_found(client_sock, resources_path);
return 1;
}
else{
//mode和S_IFMT进行与操作后得到路径状态，S_IFDIR为目录状态
if((file_path_status.st_mode & S_IFMT) == S_IFDIR){
strcat(resources_path, "/index.html");
}
}

cout<<"sending "<<resources_path<<endl;
send_server_file(client_sock, resources_path);
closesocket(client_sock);
cout<<"-----------------------------------------"<<endl;
return 0;
}