// TCPClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include<stdio.h>
#include<WinSock2.h>

#pragma comment (lib,"ws2_32")
#pragma warning(disable:4996)

int main(int argc, char** argv)
{
	/*
		1. Chương trình TCPClient, kết nối đến một máy chủ xác định bởi tên miền hoặc địa chỉ IP. 
		Sau đó nhận dữ liệu từ bàn phím và gửi đến server. 
		Tham số được truyền vào từ dòng lệnh có dạng: TCPClient.exe <Địa chỉ IP/Tên miền> <Cổng>

        VD: TCPClient.exe 192.168.1.3 8000
	*/
	char ip_address[50];
	int port;

    strcpy(ip_address, argv[1]);
	sscanf(argv[2], "%d", &port);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_address);
    addr.sin_port = htons(port);

	system("pause");

	int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));
	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		printf("Ket noi khong thanh cong - %d\n", ret);
		return 1;
	}

	char buff[2048];
	ret = -1;
	ret = recv(client, buff, sizeof(buff), 0);
	buff[ret] = 0;
	printf("%s\n", buff);

	printf("Nhap du lieu chuyen sang server: ");
	char option[10];

	while (true)
	{
		fgets(buff, sizeof(buff), stdin);
		send(client, buff, strlen(buff), 1);
		buff[0] = 0;
	}


	closesocket(client);
	WSACleanup();
}
