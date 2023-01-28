//
// Created by Administrator on 2023/1/28.
//
#include <iostream>
#include <winsock2.h>
#include "http_response.h"
using namespace std;

//彩色输出
void print_color(string str, char color){
    switch (color) {
        case 'r':
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            break;
        case 'g':
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            break;
        case 'b':
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
            break;
    }
    cout<<str;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
void print_color(int str, char color){
    switch (color) {
        case 'r':
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            break;
        case 'g':
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            break;
        case 'b':
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
            break;
    }
    cout<<str;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

//错误处理
void error_die(const char *str){
    perror(str);
    print_color(&str['\n'], 'r');
    exit(1);
}

//取消实现
void unimplement(SOCKET client_socket){
    print_color("unimplemented！\n", 'r');
//    todo
}

//资源不存在处理
void resource_not_found(SOCKET client_socket, const char *file_name){
    print_color(file_name, 'r');
    print_color(" notfound!\n", 'r');
    send_header(client_socket, "404");
    ifstream in_file("resources/404.html");
    send_content(client_socket, &in_file, FALSE);
    cout<<"-----------------------------------------"<<endl;
}