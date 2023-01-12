#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;


void error_die(const char *str){
    perror(str);
    exit(1);
}


int startup(unsigned short *port){
    //网络通信初始化
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

//从套接字读取一行，返回读取数据字节数
int get_line(int sock, char *buff, int size){
    char charRecv = 0;
    int i;
    while(charRecv!='\n' && i<size-1){
        if(recv(sock, &charRecv, 1, 0) > 0){
            if(charRecv=='r'){
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
    char buff[1024], mode[225], add[225], prot[225];
//    char *p_buff = buff, *p_mode = mode, *p_prot = prot;
    int client_sock = (SOCKET)arg;
    int chars_num = get_line(client_sock, buff, sizeof(buff));
//    while(!isspace(*p_buff)){
//        *p_mode = *p_buff;
//        *p_buff++;
//        *p_mode++;
//    }
//    while(!isspace(*p_buff)){
//        *p_prot = *p_buff;
//        *p_buff++;
//        *p_prot++;
//    }

//    cout<<mode<<endl;
//    cout<<prot<<endl;
    cout<<"func:"<<__func__ <<"\tline"<<__LINE__<<":\n"<<buff;

    return 0;
}

int main() {
    unsigned short port = 8990;
    int server_sock = startup(&port);
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    cout<<"服务启动，正在监听"<<port<<"端口"<<endl;
    //循环等待提供服务
    while(1){
//        cout<<"a";
        //创建用户套接字
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_addr_len);
        if(client_sock < 0) error_die("创建客户套接字失败");
        //创建线程（windows线程）
        DWORD threadID = 0;
        CreateThread(0, 0, accept_request, (void*)client_sock, 0, &threadID);
    }
//    closesocket(server_sock);
//    return 0;
}
