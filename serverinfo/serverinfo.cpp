#include<WinSock2.h>
#include<stdio.h>

#pragma comment (lib,"ws2_32")
#pragma warning (disable:4996)

char** splitString(char* input, char* delimitor, int* count);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	message* mess = new message;
	mess->computer_name = new char();
	int port = 0;
	char server_address[256];

	// Nhập thông tin server
	printf("Server: ");
	fgets(server_address, sizeof(server_address), stdin);

	// bỏ ký tự xuống dòng bằng kết thúc xâu
	server_address[strlen(server_address) - 1] = 0;
	int count = 0;
	const char* file_name = "serverinfo.exe";

	char** strings = splitString(server_address, (char*)" ", &count);

	/*for (int i = 0; i < count; i++)
		printf("%s\n", strings[i]);*/

	if (strcmp(strings[0], file_name) != 0)
	{
		printf("Not found near: %s", strings[0]);
		return 1;
	}

	// convert char to int
	sscanf(strings[1], "%d", &port);

	if (port <= 0) {
		printf("Port not found");
		return 1;
	}

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(ADDR_ANY);
	sockaddr.sin_port = htons(port);

	// gán cấu trúc địa chỉ với socket
	bind(listener, (SOCKADDR*)&sockaddr, sizeof(sockaddr));

	// chuyển sang trạng thái chờ kết nối
	listen(listener, 5);

	// nhận kết nối
	SOCKET client = accept(listener, NULL, NULL);

	char buff[2048];
	char infor[10][256],disk_name[10],disk_space[256];
	const char* name = "Name";
	const char* space = "Space";
	printf("Message from client\n");
	int ret = recv(client, buff, sizeof(buff), 0);

	if (ret <= 0)
	{
		if (ret == -1)
			ret = WSAGetLastError();
		return 1;
	}

	if (ret < sizeof(buff))
	{
		buff[ret] = 0;
		//printf("Computer name: %s\n", buff);
		memcpy(mess->computer_name, buff, strlen(buff));
	}

	buff[0]=0;


	while (true)
	{
		ret = recv(client, buff, sizeof(buff), 0);

		if (ret <= 0)
		{
			if (ret == -1 )
				ret = WSAGetLastError();
			return 1;
		}

		if (ret < sizeof(buff))
		{
			buff[ret] = 0;
			count = 0;
			strings = splitString(buff, (char*)"0123456789:\\", &count);
			printf("%10s|",name);
			for (int i = 0; i < count; i++) {
				printf("%20s|", strings[i]);
			}

			printf("\n%10s|", space);
			strings = splitString(buff, (char*)"ABCDEFGH:\\", &count);
			for (int i = 0; i < count; i++) {
				printf("%20s|", strings[i]);
			}
		}
	}
	
	printf("\nKet thuc nhan du lieu");

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
