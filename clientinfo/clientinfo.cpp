

#include<WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include<stdio.h>
#include<string>
#include<cstring>



#pragma comment (lib,"ws2_32")
#pragma warning (disable:4996)

int isNumber(char a);
char** splitString(char* input, char* delimitors, int* count);

int main()
{
	/*std::string s = (std::to_string());
	const char* freespace = s.c_str();*/
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	char server_address[256];
	int port = 0;
	int ret = 0;

	addrinfo* addr;
	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;

	// Nhập thông tin server
	printf("Client\n");
	printf("Nhap IP hoac ten mien server: ");
	fgets(server_address, sizeof(server_address), stdin);

	// bỏ ký tự xuống dòng bằng kết thúc xâu
	server_address[strlen(server_address)-1] = 0;
	int count = 0;
	const char* file_name = "clientinfo.exe";

	char** strings = splitString(server_address, (char*)" ", &count);

	/*for (int i = 0; i < count; i++)
		printf("%s\n", strings[i]);*/

	if (strcmp(strings[0],file_name)!=0)
	{
		printf("Not found near: %s", strings[0]);
		return 1;
	}

	// convert char to int
	sscanf(strings[2], "%d", &port);

	if (port <= 0) {
		printf("Port not found");
		return 1;
	}

	if (isNumber(strings[1][0]))
	{
		sockaddr.sin_port = htons(port);
		sockaddr.sin_addr.s_addr = inet_addr(strings[1]);
	}
	else
	{
		// đang bị lỗi không dns được localhost:port
		ret = getaddrinfo(strings[1], "http", NULL, &addr);
		if (ret != 0)
		{
			printf("DNS that bai!!");
			return 1;
		}

		memcpy(&sockaddr, addr->ai_addr, sizeof(addr->ai_addr));
	}

	// tạo socket client
	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	system("pause");

	// thực hiện connect tới server 
	ret = connect(client, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
	
	if (ret==SOCKET_ERROR)
	{
	    ret = WSAGetLastError();
	    printf("Ket noi that bai - %d !!",ret);
	    return 1;
	}

	/*
	* gửi thông tin về tên máy, danh sách các ổ đĩa có trong máy, kích thước các ổ đĩa. 
	*/

	char buffer[2048];
	DWORD size = sizeof(buffer);
	if (GetComputerNameA((LPSTR)buffer, &size))
	{
		send(client,buffer,strlen(buffer),0);
	}
	printf("%s", buffer);

	char lpBuffer[100];
	DWORD test;
	DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;

	test = GetLogicalDriveStringsA(sizeof(lpBuffer), (LPSTR)lpBuffer);

	if (test != 0)
	{
		int index = 0;

			

			/*send(client, i, strlen(i), 0);*/

		for (PCHAR i = lpBuffer; *i; i+=strlen(lpBuffer)+1)
		{
			memcpy(buffer + index, i, strlen(i));
			index += strlen(i);
			if (GetDiskFreeSpaceA(i, &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters))
			{
				char temp[256];
				unsigned long totalSpace = dwBytesPerSect * dwSectPerClust * dwFreeClusters;

				sprintf(temp, "%lu", totalSpace);
				printf("\n%ul\n", totalSpace);
				temp[strlen(temp)] = 0;

				memcpy(buffer + index, temp, strlen(temp));
				index += strlen(temp);
			}
		}

		buffer[index] = 0;

		printf("\n%s\n", buffer);
	}
	else
		printf("GetLogicalDriveStrings() is failed lor!!! Error code: %d\n", GetLastError());

	// gửi data đến server
	send(client, buffer, strlen(buffer), 0);

	// đóng kết nối
	closesocket(client);

	WSACleanup();
}

int isNumber(char a) {
	if ((a >= '0' && a <= '9'))
	{
		return 1;
	}

	return 0;
}

char** splitString(char* input, char* delimitors, int* count) {
	*count = 0;
	int i = 0;
	int length = strlen(input);

	while (i < length)
	{
		while (i < length)
		{
			if (strchr(delimitors, input[i]) == NULL)
				break;

			i++;
		}

		int pre_i = i;

		while (i < length)
		{
			if (strchr(delimitors, input[i]) != NULL)
				break;

			i++;
		}

		if (i > pre_i) *count += 1;
	}

	char** strings = new char* [*count];
	int strings_index = 0;

	char* buff = new char[length];
	i = 0;

	while (i < length)
	{
		while (i < length)
		{
			if (strchr(delimitors, input[i]) == NULL)
				break;

			i++;
		}

		int j = 0;

		while (i < length)
		{
			if (strchr(delimitors, input[i]) != NULL)
				break;
			buff[j] = input[i];
			i++;
			j++;
		}

		if (j > 0)
		{
			buff[j] = 0;
			strings[strings_index] = new char[strlen(buff) + 1];

			memcpy(strings[strings_index], buff, strlen(buff) + 1);

			strings_index++;
		}


	}
	return strings;

}
// 192.168.1.2