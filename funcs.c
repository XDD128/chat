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

static handle *handleTable;
static int tableSize;
//set to size 10

/*safely allocates space*/
void *safe_malloc(size_t size){ 
    void *newobj = malloc(size);
    if (newobj == NULL){ 
		/*checks if the space was actually allocated correctly*/
        perror("Error: could not mallocate space");
	exit(-1);
    }
    return newobj;
}
/*safely reallocates space*/
void *safe_realloc(void *p, size_t size){ 
    void *newobj = realloc(p, size);
    /*checks if the space was actually reallocated correctly*/
	if (newobj == NULL){ 
        perror("Error: could not reallocate space");
	exit(-1);
    }
    return newobj;
}

void initHandle(handle hdl){
    if (NULL == (hdl = (handle)malloc(sizeof(struct handle))))
    {
        perror("init handle malloc");
        exit(-1);
    }
    hdl -> len = 0;
    memset(hdl->field, 0, 100*sizeof(char));
}
void initTable(){
    int i = 0;
    if (NULL == (handleTable = (handle *)malloc(10*sizeof(handle))))
    {
        perror("malloc");
        exit(-1);
    }
    for (i; i < tableSize; i++){
        handleTable[i] = NULL;
    }
    tableSize = 10;
}
int getNumOfHandles(){
    int i = 0;
    int numOfHandles = 0;
    for (i; i < tableSize; i++){
        if (handleTable[i] != NULL){
            numOfHandles++;
        }
    }
    return numOfHandles;
}
//check table if there exists the user, check 0 for setup, check 1 for forwarding
int checkTable(unsigned char *handle, int handleLen){
    int i = 0;
    for (i; i < tableSize; i++)
    {
        if ((handleTable[i] != NULL) && 
        (handleLen == handleTable[i] -> len) &&
        (0 == memcmp(handle, handleTable[i] -> field, handleLen))){
            return i;
        }

    }
    return 0;
}

void addTable(int socketNumber, unsigned char *field, int handleLen){
    //realloc only happens if socket over tablesize happens, but it should only happen by small increments
    int i;
    if (socketNumber > tableSize-1){
        if(NULL == (handleTable = (handle *)realloc(handleTable,(tableSize + 10)*sizeof(handle)))){
            perror("realloc");
            exit(-1);
        }
        //NULL out for realloc
        tableSize += 10;
        for (i = tableSize - 10; i < tableSize; i++){
            handleTable[i] = NULL;
        }
    }
    printf("addTable, length: %d Data: %.*s\n", handleLen, handleLen, field);
    handleTable[socketNumber] = (handle)safe_malloc(sizeof(struct handle));
    handleTable[socketNumber] -> len = handleLen;
    memcpy((handleTable[socketNumber] -> field), field, handleLen);
    
}
//delete a table entry by freeing, triggered by flag, will assume called only if exists
void delTable(int socketNumber){
    free(handleTable[socketNumber]);
    handleTable[socketNumber] = NULL;
}

void sendChatHeader(int socketNum, int flag){
    struct chat_header ch = {htons(3), flag};
    send(socketNum, &ch, 3, 0);
}


void printHandlePacket(unsigned char *packet){
    char handleLen = packet[3];
    unsigned char printableBuf[MAX_SIZE] = {0};
    memcpy(printableBuf, &(packet[4]), handleLen);

    printf("%s", printableBuf);

}
void sendHandlePacket(int socketNum, unsigned char *name, int handleLen, int flag){
    unsigned char buf[MAXBUF];
    struct chat_header ch = {htons(4+handleLen), flag};
    struct handle h = {handleLen, 0};
    memcpy(h.field, name, handleLen);
    memcpy(buf, &ch, 3);
    memcpy(&buf[3], &h, handleLen+1);
    send(socketNum, buf, handleLen + 4, 0);
    printf("Sent Packet %d\n", flag);
}
//in client, use a while loop to block until the number of handles is gone, this is null terminated, use strlen to get from argv
void sendPacket4(int socketNum, unsigned char *message, unsigned char *name){
    unsigned char buf[MAX_SIZE] = {0};
    int messageLen = strlen(&message[3]);
    int handleLen = strlen(name);
    struct chat_header ch = {htons(4 + handleLen + messageLen), 4};
    memcpy(buf, &ch, 3);
    buf[3] = (unsigned char)handleLen;
    memcpy(&buf[4], name, handleLen);
    memcpy(&buf[4 + handleLen], &(message[3]), messageLen);
    send(socketNum, buf, 4+handleLen+messageLen, 0);

}
void sendPacket5(int socketNum, unsigned char *message, unsigned char *name){
    unsigned char buf[MAX_SIZE] = {0};
    struct chat_header ch = {0, MESSAGE_FLAG};
    unsigned short currentLen = 3;
    unsigned short s;
    int numOfHandles = 0;
    unsigned char *tok;
    //skip %m, the first token
    strtok(message, " ");
    tok = strtok(NULL, " ");
    printf("%d\n", atoi(tok));
    numOfHandles = atoi(tok);//get num of handles from second token
    buf[currentLen] = (unsigned char)strlen(name);
    memcpy(&buf[currentLen+1], name, strlen(name));
    currentLen += (strlen(name) +1);
    buf[currentLen] = (unsigned char)numOfHandles;
    currentLen++;
    while(numOfHandles){ //while there are handles left
        tok = strtok(NULL, " ");
        buf[currentLen] = (unsigned char)strlen(tok);
        memcpy(&buf[currentLen+1], tok, strlen(tok));
        currentLen += (strlen(tok) + 1); //include 1 byte for the len, and the number of bytes for the handle
        numOfHandles--;
    }
    if (tok = strtok(NULL, "")){
        memcpy(&buf[currentLen], tok, strlen(tok));
        currentLen += strlen(tok);
    }

    s = htons(currentLen);
    printf("currentLen : %d, s : %d\n", currentLen, s);
    ch.pdu_len = s;
    printf("pdu_len : %d\n", ch.pdu_len);
    memcpy(buf, &ch, 3);
    send(socketNum, buf, currentLen, 0);

}
void printMessage(unsigned char *packetBuf, int packetLen){
    printf("Got to printMessage\n");
    if (packetBuf[2] == BROADCAST_FLAG){
        printf("%.*s: %.*s\n", packetBuf[3], &packetBuf[4], packetLen - (packetBuf[3] + 4), &packetBuf[packetBuf[3] + 4]);
    }
    else if (packetBuf[2] == MESSAGE_FLAG){
        printf("%.*s: ", packetBuf[3], &packetBuf[4]);
        int idx = 4 + packetBuf[3];
        int numOfHandles = packetBuf[idx++];
        while (numOfHandles){
            idx += (packetBuf[idx]+1);
            numOfHandles--;
        }
        printf("%.*s\n", (packetLen) - idx, &packetBuf[idx]);
    }
}

void parseMessageErr(unsigned char *packetBuf, int packetLen){
    printf("Error: User doesn't exist:");
    printHandlePacket(packetBuf);
    printf("\n");
}
void sendHandleNumPacket(int socketNum, int numOfHandles){
    unsigned char buf[MAXBUF];
    //
    struct chat_header ch = {htons(7), 11};
    unsigned long l = htonl(numOfHandles);
    memcpy(buf, &ch, 3);
    memcpy(&buf[3], &l, 4);
    send(socketNum, buf, 7, 0);
}
//at this point, the socket given to socketNum is main server Socket, assign new socket with accept
int processPacket1(int socketNum, unsigned char *field, int handleLen){
    //Positive response to flag 1
    if(!checkTable(field, handleLen))
    {   printf("procPack1 len: %d\n", handleLen);
        addTable(socketNum, field, handleLen);
        sendChatHeader(socketNum, 2);
        return 1;    
    }
    //error response to flag 1
    else{
        sendChatHeader(socketNum, 3);
        return 0;
    }
}
//NULL check to see if we can broadcast to that socket,            
void processPacket4(unsigned char *packetBuf, int packetLen){
    int i;
    for (i = 0; i < tableSize; i++){
        if (handleTable[i] != NULL){
            send(i, packetBuf, packetLen, 0);
        }
    }
}
//finished
void processPacket5(int socketNum, unsigned char *packetBuf, int packetLen){
    int clientSocket;
    int i = 4;//use this index to start parsing the names
    i+= packetBuf[3];
    int numOfHandles = packetBuf[i]; //handles we gotta go through
    printf("NumOfHandles %d\n", numOfHandles);
    i++;
    while (numOfHandles){
        //checktable will get us the correct socket number, if a valid one exists, 
        if(!(clientSocket = checkTable(&packetBuf[i+1], packetBuf[i]))){
            sendHandlePacket(socketNum, &packetBuf[i+1], packetBuf[i], 7);
            printf("Sent packet7 back to socket %d\n", clientSocket);
        }
        else{
            send(clientSocket, packetBuf, packetLen, 0);
            printf("Forwarded message to socket %d\n", clientSocket);
        }
        i += packetBuf[i] + 1;
        numOfHandles--;
    }
}

//finished
void processPacket8(int socketNum){
    sendChatHeader(socketNum, 9);
    delTable(socketNum);
}
//process the list of handles
void processPacket10(int socketNum){
    int i = 0;
    int d = getNumOfHandles();
    sendHandleNumPacket(socketNum, getNumOfHandles());
    printf("Sent packet 11 with number : %d\n", d);
    for(i; i<tableSize; i++){
        if (handleTable[i] != NULL){
            sendHandlePacket(socketNum, handleTable[i] -> field, handleTable[i] -> len, 12);
        }
    }
    sendChatHeader(socketNum, 13);
    printf("sent chat header\n");
}

void processPacket11(int socketNum, unsigned char *packetBuf){
    fd_set sock;
    int packetLen;
    unsigned long numOfHandles;
    memcpy(&numOfHandles, &(packetBuf[3]), 4);
    numOfHandles = ntohl(numOfHandles);
    printf("Number of Handles is %ld\n", numOfHandles);
    FD_ZERO(&sock);
    while (numOfHandles){
        FD_SET(socketNum, &sock);
        select(socketNum + 1, &sock, (fd_set*) 0, (fd_set*)0, NULL);
        packetLen = recvPacket(socketNum, packetBuf);
        printHandlePacket(packetBuf);
        printf("\n");
        numOfHandles--;
    }
    FD_SET(socketNum, &sock);
    select(socketNum + 1, &sock, (fd_set*) 0, (fd_set*)0, NULL);
    packetLen = recvPacket(socketNum, packetBuf);
    if (packetBuf[2] != 13)
        perror("didnt send packet 13 right");
}
//finished
int recvAll(int sock, unsigned char *buf, int len, int flags){
    int packetIndex = 0;
    int messageLen = 0;
    while (packetIndex < len){
			if ((messageLen = recv(sock, &buf[packetIndex], len - packetIndex, 0)) < 0)
			{
				perror("recv call");
				exit(-1);
			}
			packetIndex += messageLen;
			//printf("At Index C1: %d\n", packetIndex);
		}
    return packetIndex;
}
//add unsigned char *buf, returning 0 will terminate the program for client, erase entry in server
int recvPacket(int clientSocket, unsigned char *buf){
	
	int messageLen = 0;
	int packetIndex = 0;
	unsigned short pdu_len;
	//here, for recv, check the first byte for the size. 
	//If recv didnt get the correct size, wait and recv again.
	//now get the data from the client_socket
	if ((messageLen = recv(clientSocket, buf, 2, 0)) < 1)
	{
		// perror("recv call");
		// exit(-1);
        return 0;
	}
    // if (0 == (messageLen)){
    //     return 0;
    // }
	memcpy(&pdu_len, buf, 2);
	pdu_len = ntohs(pdu_len);
	packetIndex += messageLen;
    //already read first 2 bytes, now get rest
	printf("recv first 2 packets : %d, pdulen: %d\n", packetIndex, pdu_len);
	if (pdu_len > (MAXBUF-2)){
		printf("C1\n");
        packetIndex += recvAll(clientSocket, &buf[packetIndex], MAXBUF - packetIndex, 0);

	}
	else{
		printf("C2\n");
        packetIndex += recvAll(clientSocket, &buf[packetIndex], pdu_len-packetIndex, 0);
	}
	printf("Message received, length: %d Data: %.*s\n", pdu_len, pdu_len, buf);
    int d = 0;
    for (d; d < pdu_len; d++){
        printf("%02X", buf[d]);
    }
    printf("\n");
	return packetIndex;
}
