#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include "http_connect.h"
#include "log.h"
#include "thread.h"
using namespace std;

#define PORT 8001

 int main() {
    unsigned short port = PORT;
    int server_sock = startup(&port);
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    print_color("Service started\tListening port:", 'g');
    print_color(port, 'g');
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    //循环等待提供服务
    while(1){
        //用户套接字
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_addr_len);
        if(client_sock < 0) error_die("创建客户套接字失败");
        //创建线程（windows线程）
        DWORD threadID;
        //创建线程后进入accept_request函数，client_sock作为其参数
        CreateThread(0, 0, accept_request, (void*)(long long)client_sock, 0, &threadID);
    }
}
