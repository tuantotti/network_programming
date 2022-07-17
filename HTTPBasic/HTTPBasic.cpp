#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

void ProcessURI(char* req);
void ProcessResult(char* op, char** param, int length);
void send(char* msg);

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);
    char buf[2048];
    int ret;
    while (1)
    {
        // Chap nhan ket noi
        SOCKET client = accept(listener, NULL, NULL);
        // Nhan yeu cau
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            closesocket(client);
            continue;
        }
        // Xu ly yeu cau
        ProcessURI(buf);
        closesocket(client);
    }
}

void ProcessURI(char* req) {
    char* p1 = strstr(req, " ");
    char* p2 = strstr(p1 + 1, " ");
    int uriLength = p2 - (p1 + 2);
    char* uri = (char*)malloc(uriLength + 1);
    memcpy(uri, p1 + 2, uriLength);
    uri[uriLength] = 0;

    char op[10];
    char** param = (char**)malloc(64 * sizeof(char*));
    int count = 0;

    printf("URI:\n%s\n", uri);

    char* p = strstr(uri, "?");

    if (p == NULL)
    {
        return;
    }

    char* nvPair = strtok(p + 1, "&");

    while (nvPair != NULL)
    {
        char* p = strstr(nvPair, "=");
        // Tach phan name
        int nameLength = p - nvPair;
        char* name = (char*)malloc(nameLength + 1);
        memcpy(name, nvPair, nameLength);
        name[nameLength] = 0;
        // Tach phan value
        int valueLength = strlen(nvPair) - nameLength - 1;
        char* value = (char*)malloc(valueLength + 1);
        memcpy(value, p + 1, valueLength);
        value[valueLength] = 0;

        if (strcmp(name, (char*)"op") == 0)
        {
            memcpy(op, value, valueLength);
            op[valueLength] = 0;
        }
        else
        {
            param[count] = (char*)malloc(valueLength * sizeof(char) + 2);
            memcpy(param[count], value, valueLength);
            param[count][valueLength] = 0;
            count++;
        }
        nvPair = strtok(NULL, "&");
    }

    ProcessResult(op,param,count);
}

void ProcessResult(char* op, char** param, int length) {
    int result = 0;
    char* msg = (char*)malloc(length * length);
    strcpy(msg, (char*)"");
    if (strcmp(op,(char*)"add") == 0)
    {
        for (int i = 0; i < length; i++)
        {
            int temp = atoi(param[i]);
            result += temp;
            strcat(msg, param[i]);
            if (i<length-1)
            {
                strcat(msg, (char*)"+");
            }
        }
        strcat(msg, (char*)"=");
        char temp[20];
        sprintf(temp, "%d", result);
        strcat(msg, temp);

    }

    if (strcmp(op, (char*)"sub") == 0)
    {
        for (int i = 0; i < length; i++)
        {
            int temp = atoi(param[i]);
            result -= temp;
            strcat(msg, param[i]);
            if (i < length - 1)
            {
                strcat(msg, (char*)"-");
            }
        }
        strcat(msg, (char*)"=");
        char temp[20];
        sprintf(temp, "%d", result);
        strcat(msg, temp);
    }

    if (strcmp(op, (char*)"mul") == 0)
    {
        for (int i = 0; i < length; i++)
        {
            int temp = atoi(param[i]);
            result *= temp;
            strcat(msg, param[i]);
            if (i < length - 1)
            {
                strcat(msg, (char*)"*");
            }
        }
        strcat(msg, (char*)"=");
        char temp[20];
        sprintf(temp, "%d", result);
        strcat(msg, temp);
    }

    if (strcmp(op, (char*)"div") == 0)
    {
        for (int i = 0; i < length; i++)
        {
            int temp = atoi(param[i]);
            result /= temp;
            strcat(msg, param[i]);
            if (i < length - 1)
            {
                strcat(msg, (char*)"/");
            }
        }
        strcat(msg, (char*)"=");
        char temp[20];
        sprintf(temp, "%d", result);
        strcat(msg, temp);
    }
}

void send(char* msg, SOCKET client) {

}