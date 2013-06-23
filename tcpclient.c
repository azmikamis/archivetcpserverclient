#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define DEFAULT_PORT 2007
#define DEFAULT_PROTO SOCK_STREAM // default TCP socket type
#define BLOCK_SIZE 20

int main(int argc, char **argv)
{
    char *server_name= "localhost"; // default to localhost
    unsigned short port = DEFAULT_PORT;
    int i, loopcount;
    unsigned int addr;
    int socket_type = DEFAULT_PROTO, retval;
    struct sockaddr_in server;
    struct hostent *hp;
    WSADATA wsaData;
    SOCKET  conn_socket;

    char filename[256] = {0};
    FILE *file;
    char buf[256];

    struct stat fInfo;
    int sendlen, sent;
    char recvbuffer[128];
    char *sendbuffer;
    int offset;
    int bytesToWrite;
    int bytesWritten;
    int bytesRemaining;

    if ((retval = WSAStartup(0x202, &wsaData)) != 0)
    {
        fprintf(stderr,"Client: WSAStartup() failed with error %d\n", retval);
        WSACleanup();
        return -1;
    }
    else
        printf("Client: WSAStartup() is OK.\n");

    if (isalpha(server_name[0])) // server address is a name
    {   
        hp = gethostbyname(server_name);
    }
    else // Convert nnn.nnn address to a usable one
    { 
        addr = inet_addr(server_name);
        hp = gethostbyaddr((char *)&addr, 4, AF_INET);
    }

    if (hp == NULL )
    {
        fprintf(stderr,"Client: Cannot resolve address \"%s\": Error %d\n", server_name, WSAGetLastError());
        WSACleanup();
        exit(1);
    }
    else
        printf("Client: gethostbyaddr() is OK.\n");

    // Copy the resolved information into the sockaddr_in structure
    memset(&server, 0, sizeof(server));
    memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);
    server.sin_family = hp->h_addrtype;
    server.sin_port = htons(port);

    loopcount = 0;

    while(1)
    {
        conn_socket = socket(AF_INET, socket_type, 0); /* Open a socket */
        if (conn_socket < 0 )
        {
            fprintf(stderr,"Client: Error Opening socket: Error %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
        else
            printf("Client: socket() is OK.\n");

        printf("Client: Client connecting to: %s.\n", hp->h_name);

        if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
        {
            fprintf(stderr,"Client: connect() failed: %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }
        else
            printf("Client: connect() is OK.\n");

        file = NULL;
        while(file == NULL)
        {
            printf("File to send: ");
            memset(buf, 0, 256);
            if(!gets(buf))
                return 0;
            //if(buf[0] == 0)
            //    buf[0] = 'a'; // to prevent fatal error if user enters nothing
            file = fopen(buf, "rb");
            if(file == NULL)
                printf("File could not be opened for reading.\n");
        }
        
        strcpy(filename, buf);

        stat(filename, &fInfo);
        printf("File name %s.\n", filename);
        printf("File size %d bytes.\n", fInfo.st_size);
        
        // send file name
        bytesToWrite = strlen(filename) + 1;
        sendbuffer = (char*)malloc(bytesToWrite);
        strcpy(sendbuffer, filename);
        send(conn_socket, sendbuffer, bytesToWrite, 0);

        retval = recv(conn_socket, recvbuffer, sizeof(recvbuffer), 0);
        if (retval == SOCKET_ERROR)
        {
            fprintf(stderr,"Client: recv() failed: error %d.\n", WSAGetLastError());
            closesocket(conn_socket);
            WSACleanup();
            return -1;
        }
        else
            printf("Client: Server received filename OK.\n");

        // send file
        bytesRemaining = fInfo.st_size;
        bytesWritten = 0;
        
        while(bytesRemaining > 0)
        {
            if(bytesRemaining < BLOCK_SIZE)
                bytesToWrite = bytesRemaining;
            else
                bytesToWrite = BLOCK_SIZE;
            
            sendbuffer = (char*)malloc(bytesToWrite);
            fread(sendbuffer, 1, bytesToWrite, file);
            send(conn_socket, sendbuffer, bytesToWrite, 0);
            bytesRemaining -= bytesToWrite;
            free(sendbuffer);
        }
        
        fclose(file);
        closesocket(conn_socket);
        printf("Buffer content: \"%s\"\n", sendbuffer);

        // send the file
        //buffer = (char*)malloc(8192);
        //sent = 0;
        //while(sent < fInfo.st_size)
        //{
        //    sendlen = 0;
        //    fread(buffer, 1, 8192, file);
        //    if((8192 + sent) > fInfo.st_size)
        //    {
        //        sendlen = send(conn_socket, buffer, (fInfo.st_size - sent), 0);
        //        printf("Client: Sent data \"%s\"\n", buffer);
        //        sent += sendlen;
        //        if(sendlen < 1)
        //        {
        //            free(buffer);
        //            fclose(file);
        //            closesocket(conn_socket);
        //            return 0;
        //        }
        //        if (sendlen != (fInfo.st_size - sent))
        //            fseek(file, (sent - fInfo.st_size), SEEK_CUR);/*nor is this statement*/
        //    }
        //    else
        //    {
        //        sendlen = send(conn_socket, buffer, 8192, 0);
        //        sent += sendlen;
        //        if(sendlen < 1)
        //        {
        //            free(buffer);
        //            fclose(file);
        //            closesocket(conn_socket);
        //            return 0;
        //        }
        //        if (sendlen != 8192)
        //            fseek(file, (sendlen - 8192), SEEK_CUR);/*nor is this one, ill leave them just to be safe*/
        //    }

        //}

        //fclose(file);

        //wsprintf(Buffer, "%s from client #%d", filename, loopcount++);

        //retval = send(conn_socket, Buffer, sizeof(Buffer), 0);
        //if (retval == SOCKET_ERROR)
        //{
        //    fprintf(stderr,"Client: send() failed: error %d.\n", WSAGetLastError());
        //    WSACleanup();
        //    return -1;
        //}
        //else
        //    printf("Client: send() is OK.\n");

        //printf("Client: Sent data \"%s\"\n", Buffer);

        //retval = recv(conn_socket, Buffer, sizeof(Buffer), 0);
        //if (retval == SOCKET_ERROR)
        //{
        //    fprintf(stderr,"Client: recv() failed: error %d.\n", WSAGetLastError());
        //    closesocket(conn_socket);
        //    WSACleanup();
        //    return -1;
        //}
        //else
        //    printf("Client: recv() is OK.\n");
   
        //if (retval == 0)
        //{
        //    printf("Client: Server closed connection.\n");
        //    closesocket(conn_socket);
        //    WSACleanup();
        //    return -1;
        //}

        //printf("Client: Received %d bytes, data \"%s\" from server.\n", retval, Buffer);

        //if (loopcount >= 2)
        //    break;
    }

    closesocket(conn_socket);
    WSACleanup();

    return 0;
}