// GroupChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32")

//const MAX_CLIENT = 64;
int numClient=0;
SOCKET clients[64];

DWORD WINAPI clientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;
    clients[numClient] = client;
    numClient++;
    char buf[256];
    int ret;
    // Nhan du lieu tu client va in ra man hinh
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        char typeSent[20];
        sscanf(buf, "%s", typeSent);
        printf("Client %d sent: %s\n",client,buf);
        char* bufSent = buf + strlen(typeSent) + 1;
        
        if (strcmp(typeSent,"all")==0)
        {
            for (int i = 0; i < numClient; i++)
            {
                if (clients[i] != client)
                {
                    send(clients[i], bufSent, strlen(bufSent), 0);
                }
            }
        }
        else
        {
            int id = atoi(typeSent);
            for (int i = 0; i < numClient; i++)
            {
                if (clients[i] == id)
                {
                    send(clients[i], bufSent, strlen(bufSent) , 0);
                }
            }
        }

    }
    closesocket(client);
}
int main()
{
    // Khoi tao thu vien
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    // Tao socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);
    // Gan cau truc dia chi voi socket
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    // Chuyen sang trang thai cho ket noi
    listen(listener, 5);
    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("Client moi ket noi: %d\n", client);
        CreateThread(0, 0, clientThread, &client, 0, 0);
    }
}