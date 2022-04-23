#include<WinSock2.h>
#include<stdio.h>

#pragma comment (lib,"ws2_32")
#pragma warning (disable:4996)

typedef struct disk_info {
	char* disk_name;
	unsigned long disk_space;
	disk_info* next;
}disk_info;

struct message {
	char* computer_name;
	disk_info* head;
};

void printMessage(const message* list);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	int port = 0;

	printf("Nhap server port: ");
	scanf("%d", &port);

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

	message* mess = new message();
	mess->computer_name = new char();
	printf("%d", sizeof(mess));

	while (1)
	{
		int ret = recv(client, (char *)mess, sizeof(mess), 0);

		if (ret <= 0)
		{
			if (ret == -1)
				ret = WSAGetLastError();
			break;
		}
		printf("%d - %s|", sizeof(mess),mess->computer_name);
		printMessage(mess);
	}

	printf("\nKet thuc nhan du lieu");

}


void printMessage(const message* list) {
	disk_info* p = list->head;
	printf("Computer name: %s\n", list->computer_name);
	while (p != NULL) {
		printf("Disk name: %s - Disk space: %ul\n", p->disk_name, p->disk_space);
		p = p->next;
	}
}