#include <stdlib.h>
#include <stdio.h>
#include "FileTransfer.h"



/*Calculates bytes sent, bitrate, and timeleft and prints to screen*/
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
		printf("%d of %d Recieved.\nLast recieve length: %d\nSpeed: %dKB/s.\nTime remaining: %dm %ds\n\n", SentT, size, SentL, (speed/1024), ((size-sent)/speed)/60, ((size-sent)/speed)%60);
		time = 0;
		sent = 0;
		}

return;
}

int main()
{
char filename[256] = {0};
FILE *file;
WSADATA wsd;
int port;
int fuckedup = 0;
char buf[256];
int loop;

/*////////file to send*/
file = NULL;
while(file == NULL)
{
printf("Save file as: ");
memset(buf, 0, 256);
	if(!gets(buf)) return 0;
	if(buf[0] == 0) buf[0] = 'a';/*to prevent fatal error if user enters nothing*/
file = fopen(buf, "wb");
	if(file == NULL)printf("File could not be opened for writing.\n");
}
fclose(file);
strcpy(filename, buf);
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

WSAStartup(514, &wsd);

printf("Waiting for connection...\n");
if(RecvFile(filename, (unsigned short)port, 32000, &CalcFunc)) printf("File recieved at \"%s\".\n", filename);
else printf("Connection error.\n");

return 0;
}