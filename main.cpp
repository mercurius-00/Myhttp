#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <sys/types.h>
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

//ȡ��ʵ��
void unimplement(SOCKET client_socket){
    cout<<"unimplemented��"<<endl;
//    todo
}

void resource_not_found(SOCKET client_socket){
    cout<<"notfound!"<<endl;
//    todo
}

void send_server_file(SOCKET client_socket, const char *path){
    cout<<"send��"<<endl;
//    todo
}

//���������̺߳���
DWORD WINAPI accept_request(LPVOID arg){
    //�����׽�������
    char resources_path[100] = "resources";  //��ԴĿ¼��
    char buff[1024];    //��һ��������
    char mode[200]; //�����󷽷���
    char resource[200]; //������URL
    char protocol[200];   //������Э��汾
    int buff_pointer = 0, temp_pointer = 0;
    int client_sock = (SOCKET)arg;  //�������
    int chars_count;    //����ÿ���ַ���

    //��ȡ����
    chars_count = get_line(client_sock, buff, sizeof(buff));
    cout<<"In "<<__func__ <<" line"<<__LINE__<<":\nrequest: "<<buff;

    //����������
    while(!isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) mode[temp_pointer++] = buff[buff_pointer++];
    mode[temp_pointer] = 0;
    temp_pointer = 0;
    cout<<"mode: \""<<mode<<"\"\n";
    while(isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) buff_pointer++;

    //����URL
    while(!isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) resource[temp_pointer++] = buff[buff_pointer++];
    resource[temp_pointer] = 0;
    temp_pointer = 0;
    cout << "resource: \"" << resource << "\"\n";
    while(isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) buff_pointer++;

    //����Э��汾
    while(!isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) protocol[temp_pointer++] = buff[buff_pointer++];
    protocol[temp_pointer] = 0;
    temp_pointer = 0;
    cout << "protocol: \"" << protocol << "\"\n";
    while(isspace(buff[buff_pointer]) && temp_pointer < sizeof(mode) - 1) buff_pointer++;

    //stricom()���ڲ����ִ�Сд�Ƚ��ַ����������ַ�����ֵ(��ͬ��0)�����������Ϸ���ȡ��ʵ��
    if(stricmp(mode, "GET") && stricmp(mode, "POST")){
        unimplement(client_sock);
        return 0;
    }

    //��ȡ��Դ����·��
    if(resource[strlen(resource)-1] == '/') strcat(resources_path, strcat(resource, "index.html"));
    else strcat(resources_path, resource);

    //�ж�resources_path�Ƿ�ΪĿ¼
    struct stat file_path_status;   //���ڴ洢resources_path�ļ�״̬
    cout<<"complete path: \""<<resources_path<<"\""<<endl;
//    cout<<stat(resources_path, &file_path_status)<<":"<<strerror(errno)<<endl;
    if(stat(resources_path, &file_path_status) == -1){  //����ȡ�ļ�״̬ʧ�ܣ�����-1
        //��ȡʣ�������Ĳ���
        while(chars_count > 0 && strcmp(buff, "\n")) chars_count = get_line(client_sock, buff, sizeof(buff));
        resource_not_found(client_sock);
    }
    else{
        //mode��S_IFMT�����������õ�·��״̬��S_IFDIRΪĿ¼״̬
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
    cout<<"�������������ڼ���"<<port<<"�˿�"<<endl;
    cout<<"-------------------------------------------------------------"<<endl;
    //ѭ���ȴ��ṩ����
    while(1){
        //�û��׽���
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_addr_len);
        if(client_sock < 0) error_die("�����ͻ��׽���ʧ��");
        //�����̣߳�windows�̣߳�
        DWORD threadID = 0;
        //�����̺߳����accept_request������client_sock��Ϊ�����
        CreateThread(0, 0, accept_request, (void*)(long long)client_sock, 0, &threadID);
    }
//    closesocket(server_sock);
//    return 0;
}
