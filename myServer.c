/******************************************************************************
* tcp_server.c
*
* CPE 464 - Program 1
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

void recvFromClient(int clientSocket);
void recvPacket(int clientSocket);
int checkArgs(int argc, char *argv[]);
int recv_len(int clientSocket, unsigned char *buf);
void recv_rest(int clientSocket, unsigned char *buf);
static fd_set socketfd;



int main(int argc, char *argv[])
{
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	
	FD_ZERO(&socketfd);
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);
	FD_SET(serverSocket, &socketfd);
	select(serverSocket + 1, &socketfd, (fd_set*) 0, (fd_set*)0, NULL);
	printf("Selecting");
	// wait for client to connect
	if (FD_ISSET(serverSocket, &socketfd)){
		
		clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
	}
	

	recvPacket(clientSocket);
	
	/* close the sockets */
	close(clientSocket);
	close(serverSocket);

	
	return 0;
}
//add unsigned char *buf
void recvPacket(int clientSocket){
	unsigned char buf[MAX_SIZE];
	int messageLen = 0;
	int packetIndex = 0;
	unsigned short pdu_len;
	//here, for recv, check the first byte for the size. 
	//If recv didnt get the correct size, wait and recv again.
	//now get the data from the client_socket
	if ((messageLen = recv(clientSocket, buf, 2, 0)) < 0)
	{
		perror("recv call");
		exit(-1);
	}
	
	pdu_len = ntohs(&buf[0]);
	packetIndex += messageLen;
	printf("recv first 2 packets : %d, pdulen: %d\n", packetIndex, pdu_len);
	if (pdu_len > MAXBUF-2){
		printf("C1\n");
		while (packetIndex < MAXBUF){
			if ((messageLen = recv(clientSocket, &buf[packetIndex-1], MAXBUF-packetIndex, 0)) < 0)
			{
				perror("recv call");
				exit(-1);
			}
			packetIndex += messageLen;
			//printf("At Index C1: %d\n", packetIndex);
		}
	}
	else{
		printf("C1\n");
		while (packetIndex < pdu_len){
			if ((messageLen = recv(clientSocket, &buf[packetIndex-1], pdu_len-packetIndex, 0)) < 0)
			{
			perror("recv call");
			exit(-1);
			}
	
			packetIndex += messageLen;
			//("At Index C2 : %d\n", packetIndex);
		}
	}
	printf("Message received, length: %d Data: %.*s\n", pdu_len, pdu_len, buf);

}


void recvFromClient(int clientSocket)
{
	unsigned char buf[MAXBUF];
	int messageLen = 0;
	//here, for recv, check the first byte for the size. 
	//If recv didnt get the correct size, wait and recv again.
	//now get the data from the client_socket
	if ((messageLen = recv(clientSocket, buf, MAXBUF, 0)) < 0)
	{
		perror("recv call");
		exit(-1);
	}

	if (messageLen != atoi(&buf[0])){
		perror("didnt receive enough bytes yet");
		printf("received: %d bytes\n", messageLen);
		exit(-1);
	}
	printf("Message received, length: %d Data: %s\n", messageLen, buf);
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 2)
	{
		fprintf(stderr, "Usage %s [optional port number]\n", argv[0]);
		exit(-1);
	}
	
	if (argc == 2)
	{
		portNumber = atoi(argv[1]);
	}
	
	return portNumber;
}

