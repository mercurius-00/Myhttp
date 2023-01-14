#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <string.h>
//#include <sys/types.h>
#include <sys/stat.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

//������
void error_die(const char *str){
    perror(str);
    exit(1);
}

//����ͨ�ų�ʼ��(����socket)
int startup(unsigned short *port){

    WSADATA data;   //SADATA��win socketר����������
    bool startup_res = WSAStartup(MAKEWORD(1, 1), &data);   //1.1�汾Э�� ��ʼ����Ϣ���浽data
    if(startup_res) error_die("����ͨ�ų�ʼ��ʧ��");    //����ֵ��Ϊ0 ��ʼ��ʧ��

    //�����׽��֣�socket��
    //PF_INET:�׽�������Ϊ�����׽���, SOCK_STREAM:���ݴ�������Ϊ������, IPPROTO_TCP:Э��ʹ��TCPЭ��
    int server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket < 0) error_die("socket����ʧ��");    //����ֵΪ-1 ��ʼ��ʧ��

    //�����׽�������(ʵ�ֶ˿ڿɸ���)
    int opt = 1;
    //setsockopt(Ҫ���õ��׽���, level, Ҫ���õ�����, �������ݵ�ַ(const char*)�� �������ݳ���)
    int setopt_res = setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
    if(setopt_res < 0) error_die("�����׽�������ʧ��");

    //���÷������������ַ
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr)); //��addr�ڴ�ȫ����Ϊ0����ʼ����
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(*port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    //���׽���������������ַ
    int bind_res = bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));
    if(bind_res < 0) error_die("�׽���������������ַ��ʧ��");

    //��̬����ӿ�
    if(*port == 0){
        int addr_len = sizeof(server_addr);
        //getsockname�����Զ�����˿ںŸ�ֵ��server_addr
        if(getsockname(server_socket, (sockaddr*)&server_addr, &addr_len) < 0) error_die("��̬����˿�ʧ��");
        *port = server_addr.sin_port;
    }

    //������������
    if(listen(server_socket, 5)) error_die("������������ʧ��");

    return server_socket;


}

//���׽��ֶ�ȡһ��(���ض�ȡ�����ֽ���)
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

//ȡ��ʵ��
void unimplement(SOCKET client_socket){
    cout<<"unimplemented��"<<endl;
//    todo
}

//��Դ�����ڴ���
void resource_not_found(SOCKET client_socket, const char *file_name){
    cout<<"notfound!"<<endl;
//    todo
}

//������Ӧ��ͷ��Ϣ
void send_header(SOCKET client_socket){
    char buff[1024];
    strcpy(buff, "HTTP/1.1 200 OK\r\n");
    send(client_socket, buff, strlen(buff), 0);
    strcpy(buff, "Server: MercuriusHttpd/0.1\r\n");
    send(client_socket, buff, strlen(buff), 0);
    strcpy(buff, "Content-type:text/html\r\n");
    send(client_socket, buff, strlen(buff), 0);
    strcpy(buff, "\r\n");
    send(client_socket, buff, strlen(buff), 0);
}

//������Ӧ����Դ����
void send_content(SOCKET client_socket, ifstream *file){
    char buff[4096];
    int count = 0;
    string line;
    while(getline(*file, line)){
        if(line.empty()) continue;
        else{
            strcpy(buff, line.data());
            send(client_socket, buff, strlen(buff), 0);
            count += line.length();
        }
    }
    cout<<count<<" byte has been sent"<<endl;
}

//�����������Դ�ļ�
void send_server_file(SOCKET client_socket, const char *file_name){
    //��ȡ��Դ�ļ�
    ifstream in_file;
    in_file.open(file_name);
    if (in_file.is_open()){
        send_header(client_socket);
        send_content(client_socket, &in_file);
    }
    else resource_not_found(client_socket, file_name);
    in_file.close();

}

//���������̺߳���
DWORD WINAPI accept_request(LPVOID arg){
    //�����׽�������
    char resources_path[100] = "resources";  //��ԴĿ¼��
    char head[1024];
    char buff[1024];    //��һ��������
    char mode[200]; //�����󷽷���
    char resource[200]; //������URL
    char protocol[200];   //������Э��汾
    int head_pointer = 0, temp_pointer = 0;
    int client_sock = (SOCKET)arg;  //�������
    int chars_count;    //����ÿ���ַ���

    //��ȡ����
    chars_count = get_line(client_sock, head, sizeof(head));
    while(chars_count > 0 && strcmp(buff, "\n")){
        chars_count = get_line(client_sock, buff, sizeof(buff));
    }
    cout<<"In thread "<<GetCurrentThreadId()<<":\nrequest: "<<head;
//    cout<<"In "<<__func__ <<" line"<<__LINE__<<":\nrequest: "<<head;

    //����������
    while(!isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) mode[temp_pointer++] = head[head_pointer++];
    mode[temp_pointer] = 0;
    temp_pointer = 0;
//    cout<<"mode: \""<<mode<<"\"\n";
    while(isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) head_pointer++;

    //����URL
    while(!isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) resource[temp_pointer++] = head[head_pointer++];
    resource[temp_pointer] = 0;
    temp_pointer = 0;
//    cout << "resource: \"" << resource << "\"\n";
    while(isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) head_pointer++;

    //����Э��汾
    while(!isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) protocol[temp_pointer++] = head[head_pointer++];
    protocol[temp_pointer] = 0;
    temp_pointer = 0;
//    cout << "protocol: \"" << protocol << "\"\n";
    while(isspace(head[head_pointer]) && temp_pointer < sizeof(mode) - 1) head_pointer++;

    //stricom()���ڲ����ִ�Сд�Ƚ��ַ����������ַ�����ֵ(��ͬ��0)�����������Ϸ���ȡ��ʵ��
    if(stricmp(mode, "GET") && stricmp(mode, "POST")){
        unimplement(client_sock);
        return 1;
    }

    //��ȡ��Դ����·��
    if(resource[strlen(resource)-1] == '/') strcat(resources_path, strcat(resource, "index.html"));
    else strcat(resources_path, resource);

    //�ж�resources_path�Ƿ�ΪĿ¼
    struct stat file_path_status;   //���ڴ洢resources_path�ļ�״̬
    if(stat(resources_path, &file_path_status) == -1){  //����ȡ�ļ�״̬ʧ�ܣ�����-1
        resource_not_found(client_sock, resources_path);
        return 1;
    }
    else{
        //mode��S_IFMT�����������õ�·��״̬��S_IFDIRΪĿ¼״̬
        if((file_path_status.st_mode & S_IFMT) == S_IFDIR){
            strcat(resources_path, "/index.html");
        }
    }

    cout<<"sending "<<resources_path<<endl;
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
    cout<<"Service started, listening port:"<<port<<endl;
    cout<<"-------------------------------------------------------------"<<endl;
    //ѭ���ȴ��ṩ����
    while(1){
        //�û��׽���
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_addr_len);
        if(client_sock < 0) error_die("�����ͻ��׽���ʧ��");
        //�����̣߳�windows�̣߳�
        DWORD threadID;
        //�����̺߳����accept_request������client_sock��Ϊ�����
        CreateThread(0, 0, accept_request, (void*)(long long)client_sock, 0, &threadID);
    }
}
