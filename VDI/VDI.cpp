// VDI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include<WinSock2.h>

#include<WS2tcpip.h>

#pragma comment (lib,"ws2_32")

/* define _WINSOCK_DEPRECATED_NO_WARNINGS to disable deprecated API warnings	*/
#pragma warning(disable:4996) 

int main()
{
	WSADATA wsa; // biến chứa thông tin về WINSOCK cài đặt trong hệ thống stands for Window Socket Application DATA
	WORD wVersion = MAKEWORD(2, 2); // tạo phiên bản WINSOCK 2.2
	
	/* WSAStartup hàm khởi tạo WINSOCK: trả về 0 nếu thành công, ngược lại trả về SOCKET_ERROR */
	int status= WSAStartup(wVersion, &wsa); 
	

	if (status == 0)
	{
		printf("start up winsock successfully\n");
	}
	else
	{
		printf("start up winsock failture\n");
	}

	// DNS
	addrinfo* info;
	int ret = getaddrinfo("localhost", "http", NULL, &info);

	if (ret == 0)
	{
		SOCKADDR_IN addr;
		memcpy(&addr, info->ai_addr, info->ai_addrlen);
		printf("Domain name: 'localhost' --> IP Address: %s\n", inet_ntoa(addr.sin_addr));
	}

	/* Giải phóng WINSOCK: trả về 0 nếu thành công, ngược lại trả về SOCKET_ERROR */
	status=WSACleanup();

	if (status == 0)
	{
		printf("clean up winsock successfully\n");
	}
	else
	{
		printf("clean up winsock failture\n");
	}	
}
