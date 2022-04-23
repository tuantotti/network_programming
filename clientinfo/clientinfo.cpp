

#include<WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
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

int isNumber(char a);
message* initMessage(char* name);
void addnew(message* list, disk_info* newMessage);
void printMessage(const message* list);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	char server_address[256];
	int port = 0;
	int ret = 0;
	message *mess=new message();

	addrinfo* addr;
	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;

	// Nhập thông tin server
	printf("Nhap IP hoac ten mien server: ");
	scanf("%s", &server_address);

	if (isNumber(server_address[0]))
	{
		printf("Nhap port cua server: ");
		fflush(stdin);
		scanf("%d", &port);
		sockaddr.sin_port = htons(port);
		sockaddr.sin_addr.s_addr = inet_addr(server_address);
	}
	else
	{
		// đang bị lỗi không dns được localhost:port
		ret = getaddrinfo(server_address, "http", NULL, &addr);
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

	char buffer[256];
	DWORD size = sizeof(buffer);
	if (GetComputerNameA((LPSTR)buffer, &size))
	{
		mess = initMessage(buffer);
	}
	char lpBuffer[100];
	DWORD test;
	DWORD dwSectPerClust, dwBytesPerSect, dwFreeClusters, dwTotalClusters;

	test = GetLogicalDriveStringsA(sizeof(lpBuffer), (LPSTR)lpBuffer);
	if (test != 0)
	{
		for (PCHAR i = lpBuffer; *i; i+=strlen(lpBuffer)+1)
		{
			if (GetDiskFreeSpaceA(i, &dwSectPerClust, &dwBytesPerSect, &dwFreeClusters, &dwTotalClusters))
			{
				unsigned long totalSpace = dwBytesPerSect * dwSectPerClust * dwFreeClusters;

				disk_info *info = new disk_info();
				info->disk_name = new char();
				memcpy(info->disk_name, i, strlen(i) + 1);
				info->disk_space = totalSpace;

				addnew(mess, info);
			}
		}
	}
	else
		printf("GetLogicalDriveStrings() is failed lor!!! Error code: %d\n", GetLastError());

	printf("%d", sizeof(mess));
	// hiển thị mess
	printMessage(mess);

	// gửi data đến server
	send(client, (char*)mess, sizeof(mess), 0);

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

message* initMessage(char *name) {
	//CONTACTLIST* newList = (CONTACTLIST*)malloc(sizeof(CONTACTLIST));
	message* newList = new message();
	newList->head = NULL;
	newList->computer_name = new char();
	memcpy(newList->computer_name, name, strlen(name) + 1);

	return newList;
}

void addnew(message* list, disk_info* newMessage) {
	//TH1 danh sach ban dau rong -> them vao dau 
	if (list->head == NULL) {
		list->head = newMessage;
		newMessage->next = NULL;
	}
	else // TH2 danh sach ban dau khong phai rong
	{
		//1 tim vi tri chen 
		if (strcmp(list->head->disk_name, newMessage->disk_name) > 0) {
			newMessage->next = list->head;
			list->head = newMessage;
		}
		else {
			disk_info* pre = list->head;
			while (pre->next != NULL && strcmp(list->head->disk_name, newMessage->disk_name) < 0)
				pre = pre->next;
			newMessage->next = pre->next;
			pre->next = newMessage;
		}
	}
}

void printMessage(const message* list) {
	disk_info* p = list->head;
	printf("Computer name: %s\n", list->computer_name);
	while (p != NULL) {
		printf("Disk name: %s - Disk space: %ul\n",p->disk_name,p->disk_space);
		p = p->next;
	}
}

// 192.168.1.2