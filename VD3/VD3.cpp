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

	SOCKADDR_IN clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	// Lay socket cua client vua ket noi den
	SOCKET client = accept(listener, (SOCKADDR*)&clientAddr, &clientAddrLen);

	char buf[256];

	while (true)
	{
		printf("Nhap ky tu: ");
		fgets(buf, sizeof(buf), stdin);

		send(client, buf, strlen(buf), 0);
	}

	closesocket(client);
}