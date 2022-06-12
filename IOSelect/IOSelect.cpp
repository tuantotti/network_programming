#include<stdio.h>
#include<winsock.h>

#pragma comment(lib, "ws2_32")
#pragma warning (disable: 4996)

typedef struct clientInfor
{
    char clientId[32];
    SOCKET client;
}CLIENT_INFO;

void RemoveClient(CLIENT_INFO* clients, int* numClients, int i)
{
    // Xoa client khoi mang
    if (i < *numClients - 1)
        clients[i] = clients[*numClients - 1];

    *numClients--;
}

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

    CLIENT_INFO clients[64];
    int numclients = 0;
    char buf[256];

    CLIENT_INFO connected[64];
    int numConnected = 0;
    char id[32], cmd[32], tmp[32];

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

        if (ret > 0 )
        {
            if (FD_ISSET(listener, &fdRead)) {
                // check new access
                SOCKET client = accept(listener, NULL, NULL);
                printf("new client: %d\n", client);

                clients[numclients].client = client;
                numclients++;
            }

            for (int i = 0; i < numclients; i++)
            {
                if (FD_ISSET(clients[i].client, &fdRead)) {
                    ret = recv(clients[i].client, buf, sizeof(buf), 0);

                    if (ret <= 0)
                    {
                        // remove client and shift left array 
                        RemoveClient(clients, &numclients, i);
                        i--;
                        continue;
                    }

                    buf[ret] = 0;
                    printf("Data from client %d: %s\n", clients[i].client, buf);

                    SOCKET client = clients[i].client;
                    int j = 0;
                    for (; j < numConnected; j++)
                        if (clients[j].client == client) break;

                    if (j==numConnected)
                    {
                        ret = sscanf(buf, "%s %s %s", cmd, id, tmp);

                        if (ret != 2)
                        {
                            const char* msg = "Command not found! Retype please.\n";
                            send(client, msg, strlen(msg), 0);
                        }
                        else
                        {
                            if (strcmp(cmd, "client_id:") != 0)
                            {
                                const char* msg = "Command not found! Retype please.\n";
                                send(client, msg, strlen(msg), 0);
                            }
                            else
                            {
                                const char* msg = "Login successfully\n";
                                send(client, msg, strlen(msg), 0);

                                connected[numConnected].client = client;
                                strcpy(connected[numConnected].clientId, id);
                                numConnected++;
                            }
                        }

                    }
                    else
                    {
                        char sbuf[256];
                        sprintf(sbuf, "%s: %s", connected[j].clientId, buf);
                        for (int k = 0; k < numConnected; k++)
                            if (client != connected[k].client)
                                send(connected[k].client, sbuf, strlen(sbuf), 0);

                        sbuf[0] = 0;
                    }

                }

            }

        }
    }

}
