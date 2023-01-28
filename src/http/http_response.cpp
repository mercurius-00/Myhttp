//
// Created by Administrator on 2023/1/28.
//
#include <iostream>
#include <string.h>
#include <winsock2.h>
#include <log.h>
#include <fstream>
using namespace std;



const char *get_content_type(const char *file_name){
    const char *p = file_name, *file_type;
    while(*p++) if(*p=='.') file_type = p+1;
    if(!strcmp(file_type, "html")) file_type="text/html";
    else if(!strcmp(file_type, "css")) file_type="text/css";
    else if(!strcmp(file_type, "jpg")) file_type="image/jpeg";
    else if(!strcmp(file_type, "jpeg")) file_type="image/jpeg";
    else if(!strcmp(file_type, "png")) file_type="image/png";
    else if(!strcmp(file_type, "ico")) file_type="image/ico";
    else if(!strcmp(file_type, "gif")) file_type="image/gif";
    else if(!strcmp(file_type, "js")) file_type="application/x-javascript";
    else(error_die("content type文件类型不存在"));
    return file_type;
}

//发送响应包头信息
void send_header(SOCKET client_socket, const char *resource_type){
    char buff[1024], content_type[20] = "Content-type:";
    if(strcmp(resource_type, "404")) strcpy(buff, "HTTP/1.1 404 NOT FOUND\r\n");
    else strcpy(buff, "HTTP/1.1 200 OK\r\n");
    send(client_socket, buff, strlen(buff), 0);
    strcpy(buff, "Server: MercuriusHttpd/0.1\r\n");
    send(client_socket, buff, strlen(buff), 0);
    strcpy(buff, strcat(strcat(content_type, resource_type), "\r\n"));
    send(client_socket, buff, strlen(buff), 0);
    strcpy(buff, "\r\n");
    send(client_socket, buff, strlen(buff), 0);
}

//发送响应包资源内容
void send_content(SOCKET client_socket, ifstream *file, bool file_is_binary){
    int count = 0;
    int buffer_length;
    if(file_is_binary){
        file->seekg (0, file->end);
        buffer_length = file->tellg();
        file->seekg (0, file->beg);
        char * buffer = new char [buffer_length];
        file->read(buffer, buffer_length);
        send(client_socket, buffer, buffer_length, 0);
        count = file->gcount();
    }
    else{
        char * buffer = new char [4096];
        string line;
        while(getline(*file, line)){
            if(line.empty()) continue;
            else{
                strcpy(buffer, line.data());
                buffer_length = strlen(buffer);
                send(client_socket, buffer, buffer_length, 0);
                count += line.length();
            }
        }
    }
    print_color(count, 'g');
    print_color(" byte has been sent!\n", 'g');
}

//发送请求的资源文件(包含send_header()与send_content())
void send_server_file(SOCKET client_socket, const char *file_name){
    bool binary = false;
    //检测后缀名
    const char *file_type = get_content_type(file_name);
    string str = file_type;
    str = str.substr(0, str.find('/'));
    if(str=="image") binary = true;
    //读取资源文件
    ifstream in_file;
    if(binary) in_file.open(file_name, ifstream::binary);
    else in_file.open(file_name);
    if (in_file.is_open()){
        send_header(client_socket, file_type);
        send_content(client_socket, &in_file, binary);
    }
    else resource_not_found(client_socket, file_name);
    in_file.close();
}