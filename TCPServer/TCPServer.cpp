// TCPServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include<stdio.h>
#include<WinSock2.h>

#pragma comment (lib,"ws2_32")
#pragma warning(disable:4996)

int main(int argc, char** argv)
{
	/*
		2. Chương trình TCPServer, đợi kết nối ở cổng xác định bởi tham số dòng lệnh. 
		Mỗi khi có client kết nối đến, thì gửi xâu chào chứa trong một tệp tin, 
		sau đó ghi toàn bộ nội dung client gửi đến vào một tệp tin khác. 
		Tên của tập tin chứa xâu chào và tập tin chứa nội dung client gửi đến được chỉ ra trong tham số dòng lệnh.
		TCPServer.exe <Cổng> <Tệp tin chứa câu chào> <Tệp tin lưu nội dung client gửi đến>

		VD: TCPServer.exe 8000 Server.txt Client.txt
	*/

	int port;
	sscanf(argv[1], "%d", &port);

	char fileName[20], clientName[20];
	char filePath[100] = "D:\\Network-Programming\\SimpleC-Cplus\\VDI\\TCPServer\\";
	char clientPath[100] = "D:\\Network-Programming\\SimpleC-Cplus\\VDI\\TCPServer\\";

	strcpy(fileName, argv[2]);
	strcpy(clientName, argv[3]);

	strcat(filePath, fileName);
	strcat(clientPath, clientName);

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	bind(listener, (sockaddr*)&addr, sizeof(addr));

	listen(listener, 5);

	printf("listening on %d ...", port);

	SOCKET client = accept(listener, NULL, NULL);

	FILE* f = fopen(filePath, "r");
	char buff[2048];
	int ret;

	while (!feof(f))
	{
		ret = fread(buff, 1, sizeof(buff), f);
		if (ret > 0)
		{
			buff[ret] = 0;
			send(client, buff, ret, 0);
		}
	}

	buff[0] = 0;
	ret = 0;
	fclose(f);

	f = fopen(clientPath, "w");
	printf("\nDu lieu tu client\n");
	while (true)
	{
		ret = recv(client, buff, sizeof(buff), 0);

		if (ret<=0)
		{
			break;
		}
		if (ret < sizeof(buff)) {

			buff[ret] = 0;
		}
		printf("%s\n", buff);

		fwrite(buff, 1, ret, f);
		fprintf(f, "\n");
	}

	fclose(f);

	// Dong ket noi va giai phong thu vien
	closesocket(client);
	closesocket(listener);
	WSACleanup();


}
