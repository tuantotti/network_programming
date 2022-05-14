// 09-05-2022-Bai1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <WinSock2.h>

#pragma comment (lib,"ws2_32")
#pragma warning (disable:4996)
   /*
   Bài 1: Lập trình server chấp nhận kết nối từ các client và
   vào vòng lặp hỏi tên client cho đến khi client gửi đúng cú pháp “client_id: xxxxxx” trong đó xxxxxx là tên của client.
   Khi client gửi đúng cú pháp thì chuyển sang vòng lặp nhận dữ liệu từ client và gửi dữ liệu đó đến các client còn lại,
   ví dụ: client có id “abc” gửi “xin chào” thì các client khác sẽ nhận được: “abc: xin chao”.
   */

int numClient = 0;
SOCKET clients[64];
const char errorCommand[] = "Nhap sai dinh dang!!Vui long nhap lai\n";
const char delim[] = ": ";
const char typeMsg[] = "Send message: ";

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

	while (true)
	{
		ret = recv(client, buff, sizeof(buff), 0);

		if (ret <= 0)
			break;

		buff[ret] = 0;

		
		if (strstr(buff, delim) == NULL)
		{
			for (int i = 0; i < numClient; i++)
			{
				if (clients[i] == client)
				{
					send(clients[i], errorCommand, strlen(errorCommand), 0);
				}
			}
		}
		else
		{
			while (true)
			{
				for (int i = 0; i < numClient; i++)
				{
					if (clients[i] == client)
					{
						send(clients[i], typeMsg, strlen(typeMsg), 0);
						break;
					}
				}
				char* token = strtok(buff, ": ");
				strcpy(msg, token);
				//strcpy(client_name, token + strlen(msg) + strlen(delim));

				buff[0] = '\0';
				ret = recv(client, buff, sizeof(buff), 0);
				buff[ret] = 0;

				strcat(msg, delim);
				strcat(msg, buff);

				//msg[strlen(buff) + strlen(delim) + 1] = 0;
				for (int i = 0; i < numClient; i++)
				{
					if (clients[i] != client)
					{
						send(clients[i], msg, strlen(msg), 0);
					}
				}
			}
		}
	}
}
