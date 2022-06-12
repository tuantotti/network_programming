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

	listen(listener, 5); // 5= so luong hang cho ket noi

	char buff[2048];
	int ret;

	while (true)
	{
		SOCKET client = accept(listener, NULL, NULL);
		ret = recv(client, buff, sizeof(buff), 0);

		if (ret>0)
		{
			if (ret < sizeof(buff)) buff[ret] = 0;
			printf("%s\n", buff);
		}

		const char* msg = "HTTP 1.1 200 0K\nContent-Type: text/html\n\n<html><body><h1>Hello world</h1></body></html>";

		send(client, msg, sizeof(msg), 0);

		
	}
	closesocket(client);

	WSACleanup();
}

// note: khi kết nối bị đóng bởi lệnh closesocket --> recv return 0(đóng 1 cách hợp lệ)
// ngắt server --> recv return -1