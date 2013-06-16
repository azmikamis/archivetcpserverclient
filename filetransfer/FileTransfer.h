/*coded by Andrew Downing*/
				 
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#include <sys/stat.h>

/*//////////Globals*/
static const int WAIT_CALL = 10;
typedef void (*MYFUNC)(int, int, int);
/*//////////End

/*//////////Declarations*/
int SendFile(const char *fName, const char* fRemoteName, unsigned int IP, unsigned short Port, int bSize, BOOL Blocking, MYFUNC CallbackFunc);
int RecvFile(const char *fName, unsigned short Port, int bSize, MYFUNC CallbackFunc);
/*//////////End*/


int SendFile(const char *fName, const char* fRemoteName, unsigned int IP, unsigned short Port, int bSize, BOOL Blocking, MYFUNC CallbackFunc)
{
char *Block;
struct sockaddr_in addr;
int s;
FILE *file;
int fNameLen;
struct stat fInfo;
int sendlen;
int sent;
int counter = 0;
int c;

s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
addr.sin_family = AF_INET;
addr.sin_port = htons(Port);
addr.sin_addr.s_addr = IP;

stat(fName, &fInfo);
	if(fInfo.st_size < 1) return 0;
	
	if(Blocking)
	{
	c = SOCKET_ERROR;
		while(connect(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
	}
	else if(connect(s, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
	closesocket(s);
	return 0;	
	}


/*////////////////////////////////////////////////////////////////Send file information to RecvFile*/
sendlen = 0;
sent = 0;
	while(sent < 4)/*loop not really neccisary*/
	{
	sendlen = send(s, (char*)&fInfo.st_size + sent, 4 - sent, 0);
		if(sendlen < 1)
		{
		closesocket(s);
		return 0;
		}
	sent += sendlen;
	}

	if(fRemoteName != NULL)
	{
	fNameLen = strlen(fRemoteName);
	sendlen = 0;
	sent = 0;
		while(sent < 4)/*nor this one*/
		{
		sendlen = send(s, (char*)&fNameLen + sent, 4 - sent, 0);
			if(sendlen < 1)
			{
			closesocket(s);
			return 0;
			}
		sent += sendlen;
		}
	sendlen = 0;
	sent = 0;
		while(sent < fNameLen)/*nor is this one*/
		{
		sendlen = send(s, fRemoteName+sent, fNameLen-sent, 0);
			if(sendlen < 1)
			{
			closesocket(s);
			return 0;
			}
		sent += sendlen;
		}
	}
/*//////////////////////////////////////////////////////////////////////////////////End*/

	if( (file = fopen(fName, "rb")) == NULL)
	{
	fclose(file);
	closesocket(s);
	return 0;
	}

	if((Block  = (char*)malloc(bSize)) == NULL)
	{
	fclose(file);
	closesocket(s);
	return 0;
	}

sent = 0;

/*////////////////////////////////////////////////////////////////////Send the file*/
	while(sent < fInfo.st_size)
	{
	sendlen = 0;
	fread(Block, 1, bSize, file);
		if((bSize + sent) > fInfo.st_size)
		{
		sendlen = send(s, Block, (fInfo.st_size - sent), 0);
		sent += sendlen;
			if(sendlen < 1)
			{
			free(Block);
			fclose(file);
			closesocket(s);
			return 0;
			}
			if (sendlen != (fInfo.st_size - sent)) fseek(file, (sent - fInfo.st_size), SEEK_CUR);/*nor is this statement*/
		}
		else
		{
		sendlen = send(s, Block, bSize, 0);
		sent += sendlen;
			if(sendlen < 1)
			{
			free(Block);
			fclose(file);
			closesocket(s);
			return 0;
			}
			if (sendlen != bSize) fseek(file, (sendlen - bSize), SEEK_CUR);/*nor is this one, ill leave them just to be safe*/
		}
		if((CallbackFunc != NULL) && (counter > WAIT_CALL))/*dont call the function every iteration to speed things up*/
		{
		counter = -1;
		(*CallbackFunc)(sent, sendlen, fInfo.st_size);
		}
	counter++;
	}
/*///////////////////////////////////////////////////////////////////////End*/

if(CallbackFunc != NULL) (*CallbackFunc)(sent, sendlen, fInfo.st_size);

fclose(file);
free(Block);
closesocket(s);

return 1;
}




int RecvFile(const char *fName, unsigned short Port, int bSize, MYFUNC CallbackFunc)
{
char *Buff;
char FileName[256] = {0};
int fSize = 0;
struct sockaddr_in addr;
int s;
int temps;
FILE *file;
int fNameLen;
int recvlen;
int recved;
int counter = 0;

s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
addr.sin_family = AF_INET;
addr.sin_port = htons(Port);
addr.sin_addr.s_addr = INADDR_ANY;

bind(s, (struct sockaddr*)&addr, sizeof(addr));
listen(s, 1);

temps = INVALID_SOCKET;

	while(temps == INVALID_SOCKET) temps = accept(s, NULL, NULL);

s = temps;
/*////////////////////////////////////////////////////////////////////Recieve file information from SendFile*/
recvlen = 0;
recved = 0;

	while(recved < 4)/*same deal as in SendFile*/
	{
	recvlen = recv(s, (char*)&fSize + recved, 4 - recved, 0);
		if(recvlen < 1)
		{
		closesocket(s);
		return 0;
		}
	recved += recvlen;
	}
	if(fName == NULL)
	{
	recvlen = 0;
	recved = 0;
		while(recved < 4)/*same deal*/
		{
		recvlen = recv(s, (char*)&fNameLen + recved, 4 - recved, 0);
			if(recvlen < 1)
			{
			closesocket(s);
			return 0;
			}
		recved += recvlen;
		}
	recvlen = 0;
	recved = 0;
		while(recved < fNameLen)/*same deal*/
		{
		recvlen = recv(s, FileName+recved, fNameLen - recved, 0);
			if(recvlen < 1)
			{
			closesocket(s);
			return 0;
			}
		recved += recvlen;
		}
	}
	else strcpy(FileName, fName);
/*/////////////////////////////////////////////////////////////////////End*/

	if( (file = fopen(FileName, "wb")) == NULL )
	{
	fclose(file);
	closesocket(s);
	return 0;	
	}

	if((Buff = (char*)malloc(bSize)) == NULL)
	{
	fclose(file);
	closesocket(s);
	return 0;	
	}

/*/////////////////////////////////////////////////////////////////Recieve the file*/
recved = 0;
	while(recved < fSize)
	{
	recvlen = 0;
	recvlen = recv(s, Buff, bSize, 0);
		if(recvlen < 1)
		{
		fclose(file);
		closesocket(s);
		free(Buff);
		return 0;
		}
	recved += recvlen;
	fwrite(Buff, 1, recvlen, file);
		if((CallbackFunc != NULL) && (counter > WAIT_CALL))/*dont call the function every iteration*/
		{
		counter = -1;
		(*CallbackFunc)(recved, recvlen, fSize);
		}
	counter++;
	}
if(CallbackFunc != NULL) (*CallbackFunc)(recved, recvlen, fSize);
/*/////////////////////////////////////////////////////////////////End*/

fclose(file);
closesocket(s);
free(Buff);
return 1;
}
