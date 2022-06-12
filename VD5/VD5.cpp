#include<stdio.h>
#include<WinSock2.h>

#include<WS2tcpip.h>

#pragma comment (lib,"ws2_32")
#pragma warning (disable:4996)

int main() {
	// Khoi tao winsock
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	addrinfo* infor;

	int ret = getaddrinfo("httpbin.org", "http", NULL, &infor);
	SOCKADDR_IN addr;
	if (ret!=0)
	{
		return 1;
	}
	memcpy(&addr, infor->ai_addr, infor->ai_addrlen);
	printf("IP Address: %s\n", inet_ntoa(addr.sin_addr));

	SOCKET client = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	int result = connect(client, (SOCKADDR*)&addr, sizeof(addr));

	const char* msg = "GET /get HTTP/1.1\nHost: httpbin.org\n\n";
	send(client,msg,sizeof(msg),0);

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
			printf("Du lieu nhan tu server: %s\n", buf);
		}
	}

	closesocket(client);
	WSACleanup();
}