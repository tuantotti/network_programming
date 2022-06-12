// TCPServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include<stdio.h>
#include<WinSock2.h>

#pragma comment (lib,"ws2_32")
#pragma warning(disable:4996)

int main(int argc, char* argv[])
{
	/*
		2. Chương trình TCPServer, đợi kết nối ở cổng xác định bởi tham số dòng lệnh.
		Mỗi khi có client kết nối đến, thì gửi xâu chào chứa trong một tệp tin,
		sau đó ghi toàn bộ nội dung client gửi đến vào một tệp tin khác.
		Tên của tập tin chứa xâu chào và tập tin chứa nội dung client gửi đến được chỉ ra trong tham số dòng lệnh.
		TCPServer.exe <Cổng> <Tệp tin chứa câu chào> <Tệp tin lưu nội dung client gửi đến>

		VD: TCPServer.exe 8000 Server.txt Client.txt
	*/

	char fileName[20], clientName[20];
	char filePath[100] = "D:\\Network-Programming\\SimpleC-Cplus\\VDI\\TCPServer\\";
	char clientPath[100] = "D:\\Network-Programming\\SimpleC-Cplus\\VDI\\TCPServer\\";

	strcpy(fileName, argv[1]);
	strcpy(clientName, argv[2]);

	strcat(filePath, fileName);
	strcat(clientPath, clientName);

	printf("%s,%s", fileName,clientName);
	printf("%s\n", filePath);
	printf("%s\n", clientPath);

	FILE* f = fopen(filePath, "r");
	char buff[2048];
	int ret;

	printf("Gui cau chao");
	while (!feof(f))
	{
		ret = fread(buff, 1, sizeof(buff), f);
		buff[ret] = 0;
		printf("%s", buff);
	}

	//buff[0] = 0;
	fclose(f);


	// Dong ket noi va giai phong thu vien


}
