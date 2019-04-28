#include "networks.h"
#include "funcs.h"
static handle *handleTable;
static int tableSize;
//set to size 10

/*safely allocates space*/
void *safe_malloc(size_t size){ 
    void newobj = malloc(size);
    if (newobj == NULL){ 
        /*checks if the space was actually allocated correctly*/
        perror("Error: could not mallocate space");
    exit(-1);
    }
    return newobj;
}
/*safely reallocates space*/
void *safe_realloc(void *p, size_t size){ 
    void newobj = realloc(p, size);
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

//check table if there exists the user, check 0 for setup, check 1 for forwarding
int checkTable(unsigned char *handle, int handleLen){
    int i = 0;
    for (i; i < tableSize; i++)
    {
        if ((handleTable[i] != NULL) && 
        (handleLen == handleTable[i] -> len) &&
        (0 == memcmp(handle, handleTable[i] -> field, handleLen))){
            return 1;
        }

    }
    return 0;
}

void addTable(int socketNumber, unsigned char *field, int handleLen){
    //realloc only happens if socket over tablesize happens, but it should only happen by small increments
    if (socketNumber > tableSize-1){
        if(NULL == (handleTable = (handle *)realloc(handleTable,(tableSize + 10)*sizeof(handle)))){
            perror("realloc");
            exit(-1);
        }
    }
    handleTable[socketNumber] = (handle)safe_malloc(sizeof(struct handle));
    handleTable[socketNumber] -> len = handleLen;
    memcpy(handleTable[socketNumber] -> field, field, handleLen);
    
}
//delete a table entry by freeing, triggered by flag, will assume called only if exists
void delTable(int socketNumber){
    free(handleTable[socketNumber]);
    handleTable[socketNumber] = NULL;
}
void sendChatHeader(int socketNum, int flag){
    struct chat_header ch = {3, flag};
    send(socketNum, &ch, 3, 0);
}
//at this point, the socket given to socketNum is main server Socket, assign new socket with accept
void processPacket1(int socketNum, unsigned char *field, int handleLen){
    int clientSocket = 0;
    //Positive response to flag 1
    if(!checkTable(field, handleLen))
    {   clientSocket = tcpAccept(socketNum, 1);
        addTable(clientSocket, field, handleLen);
        sendChatHeader(socketNum, 2);    
    }
    //error response to flag 1
    else{
        sendChatHeader(socketNum, 3);
    }
}
void processPacket4(unsigned char *packetBuf, int packetLen){
    int i;
    for (i = 0; i < tableSize; i++){
        if (handleTable[i] != NULL){
            send(i, packetBuf, packetLen, 0);
        }
    }
}
void processPacket5(unsigned char *packetBuf, int packetLen){
    int i = 3;
    
}
//The server process only need to send, client needs to print shit
//split each task like this
int processServerPacket(int socketNum, unsigned char *packetBuf, int packetLen){
    	
        //index 2 of packetbuf is where the flag will be
        switch(packetBuf[2])
	    {
        case INIT_FLAG:
            //special case
            processPacket1(socketNum, &packetBuf[3], packetLen - 3);

        case BROADCAST_FLAG:
            processPacket4(packetBuf, packetLen);
        //if handletable[i] not NULL, forward the packet to them
        case 5:
            processPacket5(packetBuf, packetLen);
        //check 
        case 7:
        case 8:
            processPacket8();
        case 9:
        case 10:
        case 11:
	    }
    

}
