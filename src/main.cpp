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
