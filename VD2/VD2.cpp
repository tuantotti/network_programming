#include<stdio.h>
#include<WinSock2.h>

#pragma comment (lib,"ws2_32")
#pragma warning (disable:4996)

int main() {
	// Khoi tao winsock
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// Tao socket cho connection - server
	// SOCKET socket(int af, int type, int protocol);
	// af: address family - actually AF_INET (IPv4) and AF_INET6
	// type: type of SOCKET
	//		SOCK_STREAM for TCP/IP: (TCP stream-oriented)
	//		SOCK_DGRAM for UDP/IP: (UDP message-oriented)
	// protocol: IPPPROTO_TCP, IPPROTO_UDP
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Tao server address
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8000); // chuyen ve big-endian (network order)

	// Gan SOCKET voi cau truc dia chi
	bind(listener, (SOCKADDR*)&addr, sizeof(addr));

	// Chuyen socket sang trang thai cho ket noi
	int listen_status = listen(listener, 5); // 5= so luong hang cho ket noi

	printf("%d", listen_status);

	// Chap nhan ket noi 
	printf("/nDang cho ket noi");

	// Tao client address
	SOCKADDR_IN clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	// Lay socket cua client vua ket noi den
	SOCKET client = accept(listener, (SOCKADDR*)&clientAddr, &clientAddrLen); // hoạt động theo cơ chế đồng bộ

	printf("\nKet noi moi: %d, dia chi IP: %s", client, inet_ntoa(clientAddr.sin_addr));

	// Truyen du lieu

	/*
	const char* str = "Hello network programming";
	send(client, str, strlen(str), 0); // send str from server to client
	// send(client, str+6, strlen(str)+6, 0); // send tu vi tri thu 6
	*/

	/*
	char buf[256];

	for (int i = 0; i < 10; i++)
	{
		buf[i] = i + 65;
	}

	send(client, buf, 10, 0);

	float pi = 3.1415;
	send(client, (char*)&pi, sizeof(pi), 0);

	*/

	// Nhan du lieu tu client
	/*
	char buf[256];

	int ret = recv(client, buf, 6, 0);
	buf[ret] = 0;
	printf("\nDu lieu nhan duoc %d bytes: %s", ret, buf); // note: data se chua ky tu enter
	*/

	char buf[256];
	while (1)
	{
		int ret = recv(client, buf, sizeof(buf), 0);

		if (ret <= 0)
		{
			if (ret == -1)
				ret = WSAGetLastError();
			break;
		}

		if (ret < sizeof(buf))
		{
			buf[ret] = 0;
			printf("%s", buf);
		}
	}

	printf("\nKet thuc nhan du lieu");
}