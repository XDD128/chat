/******************************************************************************
* myClient.c
*
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include "networks.h"
#include "funcs.h"


#define MAXBUF 1400
#define DEBUG_FLAG 1
#define xstr(a) str(a)
#define str(a) #a

void sendToServer(int socketNum);
void checkArgs(int argc, char * argv[]);
void sendInit(int socketNum);
static fd_set socketfd;
int processClientPacket(int socketNum, unsigned char *packetBuf, int packetLen){
    	
	//index 2 of packetbuf is where the flag will be
	switch(packetBuf[2])
	{
	case INIT_OK:
		break;
	case INIT_ERR:
		perror("handle already exists, choose a different one");
		exit(-1);
	case BROADCAST_FLAG:
		printMessage(packetBuf, packetLen);
		break;
	//if handletable[i] not NULL, forward the packet to them
	//will sent packet7 as ERROR back to client
	case MESSAGE_FLAG:
		printMessage(packetBuf, packetLen);
		break;
	case MESSAGE_ERR:
		parseMessageErr(packetBuf, packetLen);
		break;
	//check 

	case EXIT_OK:
		close(socketNum);
		exit(0);
	case LIST_NUMBER:
		printf("Got to case LIST_NUMBER\n");
		//read the packetbuf for len, select loop for the socket len times, then wait
		processPacket11(socketNum, packetBuf);
		break;
	default:
		perror("For some reason it defaulted");
		exit(-1);
	}
}

void takeInput(int socketNum, unsigned char *handleName){
	unsigned char buf[MAXBUF];
	int len;
	char *tok;
	unsigned char numOfHandles;
	printf("$: ");
	fgets(buf, MAXBUF, stdin);
	len = strlen(buf) - 1;
	buf[len] = 0; //NULL out the newline character

	if (buf[0] == '%'){
		buf[1] = tolower(buf[1]);
		switch(buf[1])
		{
			case 'm':
				//
				sendPacket5(socketNum, buf, handleName);
				break;
				
			case 'b':
				//null terminated buffer
				sendPacket4(socketNum, buf, handleName);
				break;
			case 'l':
				sendChatHeader(socketNum, LIST_FLAG);
				break;
			case 'e':
				sendChatHeader(socketNum, EXIT_FLAG);
				break;
			default:
				break;
		}
	}

}
int main(int argc, char * argv[])
{
	unsigned char buf[MAX_SIZE];
	int socketNum = 0;         //socket descriptor
	int i;
	int packetLen;
	fd_set currFd;
	checkArgs(argc, argv);
	FD_ZERO(&socketfd);
	FD_SET(STDIN_FILENO, &socketfd);

	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);
	FD_SET(socketNum, &socketfd);
	sendHandlePacket(socketNum, argv[1], strlen(argv[1]), 1);
	while(1){
		currFd = socketfd;
		select(socketNum+1, &currFd, (fd_set*) 0, (fd_set*)0, NULL);
		for(i = 0; i < FD_SETSIZE; i++){
			if (FD_ISSET(i, &currFd)){
				if (i == STDIN_FILENO){
					takeInput(socketNum, argv[1]);
				}
				else if (i == socketNum){
					packetLen = recvPacket(socketNum, buf);
					if (packetLen < 1){
						close(socketNum);
						printf("Server shut down\n");
						exit(-1);
					}
					processClientPacket(socketNum, buf, packetLen);
				}
			}
		}
	}

	//takeInput(socketNum);
	//sendToServer(socketNum);
	
	close(socketNum);
	
	return 0;
}

void sendToServer(int socketNum)
{
	unsigned char sendBuf[MAXBUF];   //data buffer
	unsigned short sendLen = 0;        //amount of data to send
	int sent = 0;            //actual amount of data sent/* get the data and send it   */
	unsigned short d;
	int i;

	printf("Enter the data to send: ");
	scanf("%" xstr(MAXBUF) "[^\n]%*[^\n]", &sendBuf[2]);
	
	sendLen = strlen(&sendBuf[2]) + 1;
	d = sendLen + 2;
	printf("D is %d\n", d);
	d = htons(d);
	printf("D is %d\n", d);
	memcpy(sendBuf, &d, 2);
	printf("read: %s len: %d\n", &sendBuf[2], sendLen);
	for (i = 0; i < sendLen + 2; i++){
		printf("%02x", sendBuf[i]);
	}
	printf("\n");
	sent =  send(socketNum, sendBuf, sendLen + 2, 0);
	if (sent < 0)
	{
		perror("send call");
		exit(-1);
	}

	printf("String sent: %s \n", &sendBuf[2]);
	printf("Amount of data sent is: %d\n", sent);
}

void checkArgs(int argc, char * argv[])
{
	/* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s handle host-name port-number \n", argv[0]);
		exit(1);
	}
	//check if the handle is less than 100
}
