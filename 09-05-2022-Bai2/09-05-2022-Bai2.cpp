// 09-05-2022-Bai2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <WinSock2.h>
#include<string.h>

#pragma comment (lib,"ws2_32")
#pragma warning (disable:4996)

	/*
	Bài 2: Lập trình server yêu cầu client gửi user và pass, so sánh với file cơ sở dữ liệu là một file text,
	mỗi dòng chứa một cặp user + pass
	ví dụ:
	“admin admin
	guest nopass
	…”

	+ Nếu không tìm thấy user và pass thì báo lỗi đăng nhập, yêu cầu nhập lại.
	+ Nếu tìm thấy thì chuyển sang đợi lệnh từ client, thực hiện lệnh và trả kết quả cho client.
	Dùng hàm system(“dir c:\temp > c:\\temp\\out.txt”) để thực hiện lệnh
	+ dir c:\temp là ví dụ lệnh dir mà client gửi
	+ > c:\\temp\\out.txt để định hướng kết quả lệnh dir vào file văn bản
	*/

int numClient = 0;
SOCKET clients[64];
const char errorCommand[] = "Nhap sai username or password!!Vui long nhap lai\n";
const char successCommand[] = "Dang Nhap Thang Cong\n";
const char delim[] = " ";
const char typeMsg[] = "Send message: ";
const char resultPath[] = "D:\\Network-Programming\\SimpleC-Cplus\\VDI\\09-05-2022-Bai2\\result.txt";
const char accountPath[] = "D:\\Network-Programming\\SimpleC-Cplus\\VDI\\09-05-2022-Bai2\\account.txt";

DWORD WINAPI ClientThread(LPVOID lpParam);
int main()
{

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8000);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));

	printf("Listening on port 8000 ...");

	listen(listener, 5);

	while (true)
	{
		SOCKET client = accept(listener, NULL, NULL);
		printf("Client moi ket noi: %d\n", client);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}

}

DWORD WINAPI ClientThread(LPVOID lpParam) {
	SOCKET client = *(SOCKET*)lpParam;
	clients[numClient] = client;
	numClient++;

	char buff[256];
	int ret;
	char msg[256] = "";
	char client_name[20];
	int index = 0;
	bool check = false;

	while (true)
	{
		ret = recv(client, buff, sizeof(buff), 0);

		if (ret <= 0)
			break;

		buff[ret - 1] = 0;

		char* token = strtok(buff, delim);

		FILE* f = fopen(accountPath, "r");
		char buffer[256];
		char username[20];
		char password[20];

		strcpy(username, token);
		strcpy(password, buff + strlen(token) + strlen(delim));

		while (true)
		{
			char *foef= fgets(buffer, sizeof(buffer), f);

			if (foef == NULL)
			{
				break;
			}
			buffer[strlen(buffer) - 1] = 0;
			char usnTemp[20];
			char passTemp[20];
			token = strtok(buffer, delim);

			strcpy(usnTemp, token);
			strcpy(passTemp, buffer + strlen(token) + strlen(delim));

			if (strcmp(username, usnTemp) == 0 && strcmp(password, passTemp) == 0)
			{
				check = true;
				for (int i = 0; i < numClient; i++)
				{
					if (clients[i] == client)
					{
						send(clients[i], successCommand, strlen(successCommand), 0);
						break;
					}
				}
				break;
			}
			buffer[0] = 0;
		}

		fclose(f);

		if (check==false)
		{
			for (int i = 0; i < numClient; i++)
			{
				if (clients[i] == client)
				{
					send(clients[i], errorCommand, strlen(errorCommand), 0);
					break;
				}
			}
		}

		buff[0] = 0;
		ret = 0;

		if (check)
		{
			while (true)
			{
				ret = recv(client, buff, sizeof(buff), 0);
				if (ret <= 0)
					break;
				buff[ret - 1] = 0;

				char command[256] = "";
				strcat(command, buff);
				strcat(command, " > ");
				strcat(command, resultPath);
				system(command);

				f = fopen(resultPath, "r");

				buffer[0] = 0;
				while (true)
				{
					char* foef = fgets(buffer, sizeof(buffer), f);
					if (foef == NULL)
					{
						break;
					}
					for (int i = 0; i < numClient; i++)
					{
						if (clients[i] == client)
						{
							send(clients[i], buffer, strlen(buffer), 0);
						}
					}
					buffer[0] = 0;
				}
				fclose(f);
			}
		}
	}
}
