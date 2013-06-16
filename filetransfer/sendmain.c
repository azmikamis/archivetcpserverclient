#include <stdlib.h>
#include <stdio.h>
#include "FileTransfer.h"

void CalcFunc(int SentT, int SentL, int size)
{
int speed;
static int time = 0;
static int sent = 0;

		if(time == 0) time = GetTickCount();
		if(sent == 0) sent = SentT;
		if((GetTickCount() - time) > 2000 || SentT == size)
		{
		system("CLS");
		if(SentT != size) speed = (SentT - sent) / ((GetTickCount() - time) / 1000);
		printf("%d of %d Sent.\nLast send length: %d\nSpeed: %dKB/s.\nTime remaining: %dm %ds\n\n", SentT, size, SentL, (speed/1024), ((size-sent)/speed)/60, ((size-sent)/speed)%60);
		time = 0;
		sent = 0;
		}

return;
}

int main(int argc, char **argv)
{
char filename[256] = {0};
FILE *file;
WSADATA wsd;
int port;
unsigned int ip;
int fuckedup = 0;
char buf[256];
int loop;
BOOL torf;


/*////////file to send*/
file = NULL;
while(file == NULL)
{
printf("File to send: ");
memset(buf, 0, 256);
	if(!gets(buf)) return 0;
	if(buf[0] == 0) buf[0] = 'a';/*to prevent fatal error if user enters nothing*/
file = fopen(buf, "rb");
	if(file == NULL)printf("File could not be opened for reading.\n");
}
fclose(file);
strcpy(filename, buf);
/*///////End*/

/*////////IP*/
memset(buf, 'a', 256);/*make a invalid ip for the loops conditional statement*/
while(inet_addr(buf) == INADDR_NONE)
{
printf("IP: ");
memset(buf, 0, 256);
	if(!gets(buf)) return 0;
	if(buf[0] == 0) buf[0] = 'a';
ip = inet_addr(buf);
	if(ip == INADDR_NONE)printf("Invalid IP.\n");
}
/*///////End*/

/*////////Port*/
port = 0;/* for the loop*/
while(port < 1 || port > 65535)
{
printf("Port: ");
memset(buf, 0, 256);
fuckedup = 0;
	if(!gets(buf)) return 0;
	if(strlen(buf) <= 5)
	{
		for(loop = 0;loop < (int)strlen(buf);loop++)
		{
			if(buf[loop] < 48 || buf[loop] > 57) fuckedup = 1;
		}
	    if(!fuckedup) port = atoi(buf);
		if(port < 1 || port > 65535) printf("Invalid Port.\n");
	}
	else printf("Invalid Port.\n");
}
/*///////End*/

/*/////////////////////////////////////block?*/
fuckedup = 1;
while(fuckedup)
{
fuckedup = 0;
printf("Keep trying to connect until host is ready? y/n: ");
memset(buf, 0, 256);
	if(!gets(buf)) return 0;
	if(buf[0] == 'y' || buf[0] == 'Y')
	{
	torf = TRUE;
	printf("Waiting for connection...\n");
	break;
	}
	if(buf[0] == 'n' || buf[0] == 'N')
	{
	torf = FALSE;
	break;
	}
	else fuckedup = 1;
}
/*end*/

	WSAStartup(MAKEWORD(2,2), &wsd);

	if(SendFile(filename, NULL, ip, (unsigned short)port, 8192, torf, &CalcFunc))
	{
	printf("\"%s\" has been successfully sent.\n", filename);
	}
	else printf("Connection Error.\n\n");

return 0;
}
