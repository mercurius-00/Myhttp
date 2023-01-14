#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

//错误处理
void error_die(const char *str){
    perror(str);
    exit(1);
}

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

//从套接字读取一行(返回读取数据字节数)
int get_line(int sock, char *buff, int size){
    char charRecv = 0;
    int i = 0;
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

//取消实现
void unimplement(SOCKET client_socket){
    cout<<"unimplemented！"<<endl;
//    todo
}

void resource_not_found(SOCKET client_socket){
    cout<<"notfound!"<<endl;
//    todo
}

void send_server_file(SOCKET client_socket, const char *path){
    cout<<"send！"<<endl;
//    todo
}

//处理请求线程函数
DWORD WINAPI accept_request(LPVOID arg){
    //解析套接字请求
    char resources_path[100] = "resources";  //资源目录名
    char buff[1024];    //存一行请求报文
    char mode[200]; //存请求方法名
    char resource[200]; //存请求URL
    char protocol[200];   //存请求协议版本
    int buff_pointer = 0, temp_pointer = 0;
    int client_sock = (SOCKET)arg;  //传入参数
    int chars_count;    //报文每行字符数

    //读取首行
    chars_count = get_line(client_sock, buff, sizeof(buff));
    cout<<"In "<<__func__ <<" line"<<__LINE__<<":\nrequest: "<<buff;

    //解析方法名
    while(!isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) mode[temp_pointer++] = buff[buff_pointer++];
    mode[temp_pointer] = 0;
    temp_pointer = 0;
    cout<<"mode: \""<<mode<<"\"\n";
    while(isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) buff_pointer++;

    //解析URL
    while(!isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) resource[temp_pointer++] = buff[buff_pointer++];
    resource[temp_pointer] = 0;
    temp_pointer = 0;
    cout << "resource: \"" << resource << "\"\n";
    while(isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) buff_pointer++;

    //解析协议版本
    while(!isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) protocol[temp_pointer++] = buff[buff_pointer++];
    protocol[temp_pointer] = 0;
    temp_pointer = 0;
    cout << "protocol: \"" << protocol << "\"\n";
    while(isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) buff_pointer++;

    //stricom()用于不区分大小写比较字符串，返回字符串差值(相同则0)，若方法名合法则取消实现
    if(stricmp(mode, "GET") && stricmp(mode, "POST")){
        unimplement(client_sock);
        return 0;
    }

    //获取资源完整路径
    if(resource[strlen(resource)-1] == '/') strcat(resources_path, strcat(resource, "index.html"));
    else strcat(resources_path, resource);

    //判断resources_path是否为目录
    struct stat file_path_status;   //用于存储resources_path文件状态
    cout<<"complete path: \""<<resources_path<<"\""<<endl;
//    cout<<stat(resources_path, &file_path_status)<<":"<<strerror(errno)<<endl;
    if(stat(resources_path, &file_path_status) == -1){  //若获取文件状态失败，返回-1
        //读取剩余请求报文部分
        while(chars_count > 0 && strcmp(buff, "\n")) chars_count = get_line(client_sock, buff, sizeof(buff));
        resource_not_found(client_sock);
    }
    else{
        //mode和S_IFMT进行与操作后得到路径状态，S_IFDIR为目录状态
        if((file_path_status.st_mode & S_IFMT) == S_IFDIR){
            strcat(resources_path, "/index.html");
        }
    }


    send_server_file(client_sock, resources_path);
    closesocket(client_sock);
    cout<<"-------------------------------------------------------------"<<endl;
    return 0;
}

int main() {
    unsigned short port = 8990;
    int server_sock = startup(&port);
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    cout<<"服务启动，正在监听"<<port<<"端口"<<endl;
    cout<<"-------------------------------------------------------------"<<endl;
    //循环等待提供服务
    while(1){
        //用户套接字
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_addr_len);
        if(client_sock < 0) error_die("创建客户套接字失败");
        //创建线程（windows线程）
        DWORD threadID = 0;
        //创建线程后进入accept_request函数，client_sock作为其参数
        CreateThread(0, 0, accept_request, (void*)(long long)client_sock, 0, &threadID);
    }
//    closesocket(server_sock);
//    return 0;
}
