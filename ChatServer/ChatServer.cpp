#include <stdio.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#define CONNECT "[CONNECT]"
#define DISCONNECT "[DISCONNECT]"
#define SEND "[SEND]"
#define LIST "[LIST]"
#define ERROR "[ERROR]"
#define MESSAGE_ALL "[MESSAGE_ALL]"
#define MESSAGE "[MESSAGE]"
#define USER_CONNECT "[USER_CONNECT]"
#define USER_DISCONNECT "[USER_DISCONNECT]"

SOCKET clients[64];
char ids[64][100];
int numClient = 0;

bool checkExistAccount(char* id);
int get(char* id);
int get(SOCKET client);
void sendMessageToClient(SOCKET client, const char* protocol, const char* mess);
void sendMessageToAllClients(const char* protocol, const char* mess);
void connect(SOCKET client, char* id);
void disconnect(SOCKET client, char* content);
void sendMessage(SOCKET client, char* content);
void getLoggedInUsers(SOCKET client, char* content);
void excute(SOCKET client, char* req);
DWORD WINAPI clientThread(LPVOID lpParam);

int main()
{
    WSAData wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKADDR_IN addr;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    bind(listener, (sockaddr*)&addr, sizeof(addr));

    listen(listener, 5);

    while (1) {
        SOCKET client = accept(listener, NULL, NULL);
        clients[numClient++] = client;
        CreateThread(0, 0, clientThread, &client, 0, 0);
    }

    closesocket(listener);
    WSACleanup();
}

bool checkExistAccount(char* id) {
    if (id == NULL) return false;

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

int get(char* id) {
    if (id == NULL) return -1;

    for (int i = 0; i < numClient; i++)
        if (ids[i] != NULL && strcmp(id, ids[i]) == 0)
            return i;

    return -1;
}

int get(SOCKET client) {
    for (int i = 0; i < numClient; i++)
        if (client == clients[i])
            return i;

    return -1;
}

void sendMessageToClient(SOCKET client, const char* protocol, const char* mess) {
    if (mess == NULL) mess = "";

    char buf[1024] = "";
    strcat(buf, protocol);
    strcat(buf, " ");
    strcat(buf, mess);
    strcat(buf, "\n");

    send(client, buf, strlen(buf), 0);
}

void sendMessageToAllClients(const char* protocol, const char* mess) {
    if (mess == NULL) mess = "";

    char buf[1024] = "";

    strcat(buf, protocol);
    strcat(buf, " ");
    strcat(buf, mess);
    strcat(buf, "\n");

    for (int i = 0; i < numClient; i++)
        if (ids[i][0] != NULL)
            send(clients[i], buf, strlen(buf), 0);
}

void connect(SOCKET client, char* id) {
    if (get(id) != -1) {
        sendMessageToClient(client, CONNECT, "ERROR this id already logged in");
    }
    else if (checkExistAccount(id)) {
        strcpy(ids[get(client)], id);
        sendMessageToClient(client, CONNECT, "OK");
        sendMessageToAllClients(USER_CONNECT, id);
    }
    else {
        sendMessageToClient(client, CONNECT, "ERROR wrong id");
    }
}

void disconnect(SOCKET client, char* content) {
    sendMessageToClient(client, DISCONNECT, "OK");
    sendMessageToAllClients(USER_DISCONNECT, ids[get(client)]);
    ids[get(client)][0] = NULL;
}

void sendMessage(SOCKET client, char* content) {

    char* option = content;
    char* mess = NULL;

    for (int i = 0; i < strlen(content); i++) {
        if (content[i] == ' ') {
            content[i] = 0;
            mess = content + i + 1;
            break;
        }
    }

    char* id = ids[get(client)];

    char buf[1024] = "";

    if (id != NULL) {
        strcpy(buf, id);
        strcat(buf, " ");
    }
    if (mess != NULL) strcat(buf, mess);

    if (strcmp(option, "ALL") == 0) {
        sendMessageToAllClients(MESSAGE_ALL, buf);
        sendMessageToClient(client, SEND, "OK");
    }
    else {
        int user_recv_mess_index = get(option);
        if (user_recv_mess_index >= 0) {
            sendMessageToClient(clients[user_recv_mess_index], MESSAGE, buf);
            sendMessageToClient(client, SEND, "OK");
        }
        else {
            sendMessageToClient(client, SEND, "ERROR id does not exist");
        }
    }
}

void getLoggedInUsers(SOCKET client, char* content) {
    char client_list[1024] = "OK";
    for (int i = 0; i < numClient; i++)
        if (ids[i] != NULL) {
            strcat(client_list, " ");
            strcat(client_list, ids[i]);
        }

    sendMessageToClient(client, LIST, client_list);
}

void excute(SOCKET client, char* req) {
    if (req == NULL || strlen(req) < 1)
        return;

    char* protocol = req;
    char* content = NULL;

    for (int i = 0; i < strlen(req); i++) {
        if (req[i] == ' ') {
            req[i] = 0;
            content = req + i + 1;
            break;
        }
    }

    if (strcmp(protocol, CONNECT) == 0) {
        connect(client, content);
    }
    else if (strcmp(protocol, DISCONNECT) == 0) {
        disconnect(client, content);
    }
    else if (strcmp(protocol, SEND) == 0) {
        sendMessage(client, content);
    }
    else if (strcmp(protocol, LIST) == 0) {
        getLoggedInUsers(client, content);
    }
    else {
        //sai giao thuc
        sendMessageToClient(client, ERROR, "");
    }

}

DWORD WINAPI clientThread(LPVOID lpParam)
{
    SOCKET client = *(SOCKET*)lpParam;

    int ret;
    char buf[256];

    while (1) {
        int ret = recv(client, buf, sizeof(buf) - 1, 0);

        if (ret < 0) {
            int index = get(client);

            clients[index] = clients[numClient - 1];
            strcpy(ids[index], ids[numClient - 1]);

            numClient--;

            break;
        }

        buf[ret] = 0;
        if (buf[ret - 1] == '\n') buf[ret - 1] = 0;

        excute(client, buf);
    }

    closesocket(client);
}