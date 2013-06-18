#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_PORT 2007
#define DEFAULT_PROTO SOCK_STREAM // default TCP socket type

int main(int argc, char **argv)
{
	char Buffer[128];
    char *ip_address= NULL;
    unsigned short port=DEFAULT_PORT;
    int fromlen;
    int i;
    int socket_type = DEFAULT_PROTO, retval;
    struct sockaddr_in local, from;
    WSADATA wsaData;
    SOCKET listen_socket, msgsock;
    char *buffer;
    int recved;
    FILE *file;
    // Request Winsock version 2.2
    if ((retval = WSAStartup(0x202, &wsaData)) != 0)
    {
        fprintf(stderr,"Server: WSAStartup() failed with error %d\n", retval);
        WSACleanup();
        return -1;
    }
    else
    	printf("Server: WSAStartup() is OK.\n");

    local.sin_family = AF_INET;
    local.sin_addr.s_addr = (!ip_address) ? INADDR_ANY : inet_addr(ip_address);
    local.sin_port = htons(port); // Port MUST be in Network Byte Order
    
    listen_socket = socket(AF_INET, socket_type, 0); // TCP socket
    if (listen_socket == INVALID_SOCKET)
    {
        fprintf(stderr,"Server: socket() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    else
       printf("Server: socket() is OK.\n");

    if (bind(listen_socket, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR)
    {
        fprintf(stderr,"Server: bind() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    else
        printf("Server: bind() is OK.\n");

    if (listen(listen_socket, 5) == SOCKET_ERROR)
    {
        fprintf(stderr,"Server: listen() failed with error %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    else
        printf("Server: listen() is OK.\n");

    printf("Server: %s: I'm listening and waiting connection\non port %d, protocol %s\n", argv[0], port, (socket_type == SOCK_STREAM) ? "TCP"  :"UDP");

    while(1)
    {
    	fromlen = sizeof(from);

        msgsock = accept(listen_socket, (struct sockaddr*)&from, &fromlen);
        if (msgsock == INVALID_SOCKET)
        {
            fprintf(stderr,"Server: accept() error %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
        else
            printf("Server: accept() is OK.\n");

        printf("Server: accepted connection from %s, port %d\n", inet_ntoa(from.sin_addr), htons(from.sin_port));

        buffer = (char*)malloc(8192);
        recved = 0;
        file = fopen("input1.txt", "w");
        //while(1)
        //{
            while(recved < 1090)
            {
            	//retval = recv(msgsock, Buffer, sizeof(Buffer), 0);
                retval = recv(msgsock, buffer, 8192, 0);
                recved += retval;
                fwrite(buffer, 1, 8192, file);
            	printf("Server: Received %d bytes, data \"%s\" from client\n", retval, buffer);
            }
        //}

        fclose(file);
        closesocket(msgsock);
        free(buffer);

        //retval = recv(msgsock, Buffer, sizeof(Buffer), 0);
        //if (retval == SOCKET_ERROR)
        //{
        //    fprintf(stderr,"Server: recv() failed: error %d\n", WSAGetLastError());
        //    closesocket(msgsock);
        //    continue;
        //}
        //else
        //    printf("Server: recv() is OK.\n");

        //if (retval == 0)
        //{
        //	printf("Server: Client closed connection.\n");
        //    closesocket(msgsock);
        //    continue;
        //}

        //printf("Server: Received %d bytes, data \"%s\" from client\n", retval, Buffer);
        //printf("Server: Echoing the same data back to client...\n");

        //retval = send(msgsock, Buffer, sizeof(Buffer), 0);
        //if (retval == SOCKET_ERROR)
        //    fprintf(stderr,"Server: send() failed: error %d\n", WSAGetLastError());
        //else
        //    printf("Server: send() is OK.\n");

        //printf("Server: I'm waiting more connection, try running the client\n");
        //printf("Server: program from the same computer or other computer...\n");
        //closesocket(msgsock);

        break;
    }


	return 0;
}