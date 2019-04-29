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
            //special case
            return processPacket1(socketNum, &packetBuf[4], packetBuf[3]);
        case INIT_ERR:
            processPacket4(packetBuf, packetLen);
        //if handletable[i] not NULL, forward the packet to them
		//will sent packet7 as ERROR back to client
        case MESSAGE_ERR:
            processPacket5(socketNum, packetBuf, packetLen);
        //check 

        case EXIT_OK:
            processPacket8(socketNum);
        case LIST_NUMBER:
			processPacket10(socketNum);
        default:
            perror("For some reason it defaulted");
            exit(-1);
	    }
    

}

void takeInput(int socketNum){
	
}
int main(int argc, char * argv[])
{
	int socketNum = 0;         //socket descriptor
	
	checkArgs(argc, argv);
	
	/* set up the TCP Client socket  */
	socketNum = tcpClientSetup(argv[2], argv[3], DEBUG_FLAG);
	
	sendHandlePacket(socketNum, argv[1], strlen(argv[1]), 1);
	
	sendToServer(socketNum);
	
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
