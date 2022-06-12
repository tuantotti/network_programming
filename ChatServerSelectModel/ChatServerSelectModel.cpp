#include<stdio.h>
#include<winsock.h>

#pragma comment(lib, "ws2_32")
#pragma warning (disable: 4996)

#define ALL "ALL"
#define OKE "OKE"
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define SEND "SEND"
#define LIST "LIST"
#define ERROR "ERROR"
#define MESSAGE_ALL "MESSAGE_ALL"
#define MESSAGE "MESSAGE"
#define USER_CONNECT "USER_CONNECT"
#define USER_DISCONNECT "USER_DISCONNECT"

#define LOGIN_FAIL "Login fail."
#define LOGIN_SUCCESS "Login successfully."



typedef struct clientInfor
{
	char clientId[32];
	SOCKET client;
	bool isConnected;
}CLIENT_INFO;

CLIENT_INFO clients[64];
int numclients = 0;


void RemoveClient(int i);
void removeLoggedIn(int i);
bool isExistAccount(char* id);
bool isLoggedIn(CLIENT_INFO client);
int isLoggedIn(char* id);
void connect(CLIENT_INFO client, char* id);
void disconnect(int i);
void sendToClient(CLIENT_INFO client, char* msg);
void sendToAll(CLIENT_INFO client, char* msg);
void getCurrentLoggedIn(CLIENT_INFO client);
int get(char* id);
int get(SOCKET client);
void sendMessage(int i, char* option, char* msg);


int main()
{
	WSAData wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(listener, (sockaddr*)&addr, sizeof(addr));

	listen(listener, 5);

	fd_set fdRead;
	int ret;
	char id[32], cmd[32], tmp[32], buf[256];

	while (true)
	{
		// remove fd_read
		FD_ZERO(&fdRead);

		// add listener to read event
		FD_SET(listener, &fdRead);

		for (int i = 0; i < numclients; i++)
		{
			FD_SET(clients[i].client, &fdRead);
		}

		ret = select(0, &fdRead, 0, 0, 0);

		if (ret == SOCKET_ERROR)
		{

		}

		// timeout
		if (ret == 0)
		{

		}

		if (ret > 0)
		{
			if (FD_ISSET(listener, &fdRead)) {
				// check new access
				SOCKET client = accept(listener, NULL, NULL);
				printf("new client: %d\n", client);

				clients[numclients].client = client;
				clients[numclients].isConnected = false;
				numclients++;
			}

			for (int i = 0; i < numclients; i++)
			{
				if (FD_ISSET(clients[i].client, &fdRead)) {
					ret = recv(clients[i].client, buf, sizeof(buf), 0);

					if (ret <= 0)
					{
						// remove client and shift left array 
						RemoveClient(i);
						i--;
						continue;
					}

					buf[ret - 1] = 0;
					printf("Data from client: %s\n", buf);

					CLIENT_INFO client = clients[i];

					ret = sscanf(buf, "%s %s %s", cmd, id, tmp);


					if (isLoggedIn(client) == false)
					{
						if (strcmp(cmd, CONNECT) == 0 && ret == 2)
						{
							connect(client, id);
						}
						else if (strcmp(cmd, CONNECT) != 0)
						{
							char sbuf[256];
							sprintf(sbuf, "You need login to use: %s\n", buf);
							sendToClient(client, sbuf);
						}
					}
					else
					{
						if (strcmp(cmd, LIST) == 0 && ret == 1)
						{
							getCurrentLoggedIn(client);
						}

						if (strcmp(cmd, DISCONNECT) == 0 && ret == 1)
						{
							disconnect(i);
						}

						if (strcmp(cmd, SEND) == 0 && ret > 2)
						{
							char* msg = buf + strlen(cmd) + strlen(id) + 2;
							sendMessage(i, id, msg);
						}

						if (ret < 1 || (strcmp(cmd, LIST) != 0 && strcmp(cmd, DISCONNECT) != 0 && strcmp(cmd, SEND) != 0))
						{
							char sbuf[256];
							sprintf(sbuf, "%s %s %s\n", ERROR, cmd, "Not Found.");
							sendToClient(client, sbuf);
						}
					}



				}

			}

		}
	}

}

void RemoveClient(int i)
{
	if (i < numclients - 1) {
		clients[i] = clients[numclients - 1];
	}

	numclients--;
}

void removeLoggedIn(int i) {
	clients[i].isConnected = false;
}

bool isExistAccount(char* id) {
	FILE* f = fopen("account.txt", "r");

	char buf[256];

	while (!feof(f)) {
		fgets(buf, sizeof(buf), f);

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;

		if (strcmp(id, buf) == 0)
			return true;
	}

	return false;
}
bool isLoggedIn(CLIENT_INFO client) {
	return client.isConnected == true ? true : false;
}
int isLoggedIn(char* id) {
	for (int i = 0; i < numclients; i++)
	{
		if (strcmp(id, clients[i].clientId) == 0 && clients[i].isConnected == true)
		{
			return i;
		}
	}
	return -1;
}
void connect(CLIENT_INFO client, char* id) {
	if (isExistAccount(id))
	{
		if (isLoggedIn(id) < 0 && client.isConnected == false)
		{
			int index = get(client.client);
			strcpy(clients[index].clientId, id);
			clients[index].isConnected = true;

			char sbuf[256];
			sprintf(sbuf, "%s %s %s\n", CONNECT, OKE, LOGIN_SUCCESS);
			sendToClient(client, sbuf);

			sbuf[0] = 0;
			sprintf(sbuf, "%s %s\n", USER_CONNECT, id);
			sendToAll(client, sbuf);
		}
		else
		{
			char sbuf[256];
			sprintf(sbuf, "%s %s\n", CONNECT, (char*)"Log in currently in another device");
			sendToClient(client, sbuf);
		}

	}
	else
	{
		char sbuf[256];
		sprintf(sbuf, "%s %s %s\n", CONNECT, ERROR, LOGIN_FAIL);
		sendToClient(client, sbuf);
	}
}
void disconnect(int i) {
	CLIENT_INFO client = clients[i];
	char* id = client.clientId;

	char sbuf[256];
	sprintf(sbuf, "%s %s\n", DISCONNECT, OKE);
	sendToClient(client, sbuf);

	sbuf[0] = 0;
	sprintf(sbuf, "%s %s\n", USER_DISCONNECT, id);
	sendToAll(client, sbuf);

	removeLoggedIn(i);
}
void sendToClient(CLIENT_INFO client, char* msg) {
	send(client.client, msg, strlen(msg), 0);
}
void sendToAll(CLIENT_INFO client, char* msg) {
	for (int i = 0; i < numclients; i++)
	{
		if (clients[i].client != client.client && clients[i].isConnected == true)
		{
			send(clients[i].client, msg, strlen(msg), 0);
		}
	}
}
void sendMessage(int i, char* option, char* msg) {
	CLIENT_INFO client = clients[i];

	if (strcmp(option, ALL) == 0)
	{
		char sbuf[256];
		sprintf(sbuf, "%s %s\n", SEND, OKE);
		sendToClient(client, sbuf);

		sbuf[0] = 0;
		sprintf(sbuf, "%s %s %s\n", MESSAGE_ALL, client.clientId, msg);
		sendToAll(client, sbuf);
	}
	else
	{
		char id[32];
		strcpy(id, option);
		int targetUserIndex = isLoggedIn(id);

		if (targetUserIndex < 0)
		{
			char sbuf[256];
			sprintf(sbuf, "%s %s %s\n", SEND, ERROR, "User is not exist!");
			sendToClient(client, sbuf);
		}
		else
		{
			char sbuf[256];
			sprintf(sbuf, "%s %s %s\n", MESSAGE, client.clientId, msg);
			sendToClient(clients[targetUserIndex], sbuf);

			sbuf[0] = 0;
			sprintf(sbuf, "%s %s\n", SEND, OKE);
			sendToClient(client, sbuf);
		}
	}
}
void getCurrentLoggedIn(CLIENT_INFO client) {
	char loggedInUsers[2048] = "CURRENT LOGGED IN ";

	for (int i = 0; i < numclients; i++)
	{
		strcat(loggedInUsers, clients[i].clientId);
		strcat(loggedInUsers, " ");
	}

	strcat(loggedInUsers, "\n");
	sendToClient(client, loggedInUsers);
}

int get(char* id) {
	for (int i = 0; i < numclients; i++)
	{
		if (strcmp(clients[i].clientId, id) == 0)
		{
			return i;
		}
	}

	return -1;
}

int get(SOCKET client) {
	for (int i = 0; i < numclients; i++)
	{
		if (clients[i].client == client)
		{
			return i;
		}
	}

	return -1;
}
