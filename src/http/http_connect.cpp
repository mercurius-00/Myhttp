//
// Created by Administrator on 2023/1/28.
//
#include <winsock2.h>
#include <log.h>
using namespace std;

//网络通信初始化(返回socket)
int startup(unsigned short *port){

    WSADATA data;   //SADATA：win socket专用数据类型
    bool startup_res = WSAStartup(MAKEWORD(1, 1), &data);   //1.1版本协议 初始化信息保存到data
    if(startup_res) error_die("网络通信初始化失败");    //返回值不为0 初始化失败

    //创建套接字（socket）
    //PF_INET:套接字类型为网络套接字, SOCK_STREAM:数据传输类型为数据流, IPPROTO_TCP:协议使用TCP协议
    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket < 0) error_die("socket创建失败");    //返回值为-1 初始化失败

    //设置套接字属性(实现端口可复用)
    int opt = 1;
    //setsockopt(要设置的套接字, level, 要设置的属性, 属性内容地址(const char*)， 属性内容长度)
    int setopt_res = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    if(setopt_res < 0) error_die("设置套接字属性失败");

    //配置服务器段网络地址
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); //将addr内存全部设为0（初始化）
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(*port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //绑定套接字与服务器网络地址
    int bind_res = bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    if(bind_res < 0) error_die("套接字与服务器网络地址绑定失败");

    //动态分配接口
    if(*port == 0){
        int addr_len = sizeof(server_addr);
        //getsockname函数自动分配端口号赋值到server_addr
        if(getsockname(server_socket, (sockaddr*)&server_addr, &addr_len) < 0) error_die("动态分配端口失败");
        *port = server_addr.sin_port;
    }

    //创建监听队列
    if(listen(server_socket, 5)) error_die("创建监听队列失败");
    return server_socket;
}