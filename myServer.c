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

int checkArgs(int argc, char *argv[]);
int recv_len(int clientSocket, unsigned char *buf);
void recv_rest(int clientSocket, unsigned char *buf);
static fd_set socketfd;

//The server process only need to send, client needs to print shit
//split each task like this
int processServerPacket(int socketNum, unsigned char *packetBuf, int packetLen){
    	
        //index 2 of packetbuf is where the flag will be
        switch(packetBuf[2])
	    {
        case INIT_FLAG:
            //special case
            return processPacket1(socketNum, &packetBuf[4], packetBuf[3]);
        case BROADCAST_FLAG:
            processPacket4(socketNum, packetBuf, packetLen);
			return BROADCAST_FLAG;
        //if handletable[i] not NULL, forward the packet to them
		//will sent packet7 as ERROR back to client
        case MESSAGE_FLAG:
            processPacket5(socketNum, packetBuf, packetLen);
			return MESSAGE_FLAG;
        //check 

        case EXIT_FLAG:
            processPacket8(socketNum);
			return EXIT_FLAG;
        case LIST_FLAG:
			processPacket10(socketNum);
			return LIST_FLAG;
        default:
            perror("For some reason it defaulted");
            exit(-1);
	    }
    

}
// void processServer(int fdLen, fd_set *fds){
//     int i = 0;
//     select(serverSocket + 1, &socketfd, (fd_set*) 0, (fd_set*)0, NULL);
//     for (i; i<FD_SETSIZE; i++){
//         if (FD_ISSET(i, &socketfd)){
//             if (i == serverSocket){//unconnected socket
//                 clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
//                 packetLen = recvPacket(clientSocket, buf);
//                 if (processServerPacket(clientSocket, buf, packetLen)){
// 					FD_SET(clientSocket, &socketfd);
// 				}
// 				else{
// 					close(clientSocket);
// 				}

//             }
//             else{
//                 packetLen = recvPacket(i, buf);
//                 if (packetLen == 0){
//                     //handle with flag 9 sent to client, remove flag from global set and table, close socket
//                 }
//                 processServerPacket(i, buf, packetLen);
//             }//already connected socket
//         }

// }

int main(int argc, char *argv[])
{	unsigned char buf[MAXBUF];
	int serverSocket = 0;   //socket descriptor for the server socket
	int clientSocket = 0;   //socket descriptor for the client socket
	int portNumber = 0;
	int packetLen;
	fd_set socketfdm;
	initTable();
	FD_ZERO(&socketfd);
	portNumber = checkArgs(argc, argv);
	
	//create the server socket
	serverSocket = tcpServerSetup(portNumber);
	FD_SET(serverSocket, &socketfd);
	int maxsock = serverSocket;
	while (1){
		clientSocket = 0;
		socketfdm = socketfd;
		int i = 0;
		printf("Selecting\n");
    	select(maxsock+ 1, &socketfdm, (fd_set*) 0, (fd_set*)0, NULL);
		printf("Done selecting\n");
		for (i; i<FD_SETSIZE; i++){
			if (FD_ISSET(i, &socketfdm)){
				printf("fd is set %d\n", i);
				if (i == serverSocket){//unconnected socket
					clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
					packetLen = recvPacket(clientSocket, buf);
					printf("Line 89 Received %d\n", packetLen);
					if (processServerPacket(clientSocket, buf, packetLen)){
						FD_SET(clientSocket, &socketfd);
						if (clientSocket > maxsock){
							maxsock = clientSocket;
						}
						printf("successfully added the boy, socket: %d\n", clientSocket);
					}
					else{
						close(clientSocket);
						printf("closed\n");
					}

				}
				else{
					packetLen = recvPacket(i, buf);
					if (packetLen < 1){
						delTable(i);
						FD_CLR(i, &socketfd);
						close(i);
						printf("Close 1\n");
						//handle with flag 9 sent to client, remove flag from global set and table, close socket
					}
					else if (EXIT_FLAG == processServerPacket(i, buf, packetLen)){
						FD_CLR(i, &socketfd);
						close(i);
						printf("Close 2\n");
					}
				}//already connected socket
			}			
		}
	}
	// select(serverSocket + 1, &socketfd, (fd_set*) 0, (fd_set*)0, NULL);
	// printf("Selecting");
	// // wait for client to connect
	// if (FD_ISSET(serverSocket, &socketfd)){
		
	// 	clientSocket = tcpAccept(serverSocket, DEBUG_FLAG);
	// }
	

	//recvPacket(clientSocket);
	
	/* close the sockets */
	close(clientSocket);
	close(serverSocket);

	
	return 0;
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

