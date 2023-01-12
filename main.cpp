#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;


void error_die(const char *str){
    perror(str);
    exit(1);
}


int startup(unsigned short *port){
    //����ͨ�ų�ʼ��
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

//���������̺߳���
DWORD WINAPI accept_request(LPVOID arg){

    return 0;
}

int main() {
    unsigned short port = 0;
    int server_sock = startup(&port);
    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    cout<<"�������������ڼ���"<<port<<"�˿�"<<endl;
    //ѭ���ȴ��ṩ����
    while(1){
        //�����û��׽���
        int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_addr_len);
        if(client_sock < 0) error_die("�����ͻ��׽���ʧ��");
        //�����̣߳�windows�̣߳�
        DWORD threadID = 0;
        CreateThread(0, 0, accept_request, (void*)client_sock, 0, &threadID);
    }
    closesocket(server_sock);
    return 0;
}
