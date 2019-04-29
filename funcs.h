#ifndef FUNCS_H
#define FUNCS_H

#define MAX_SIZE 100
#define HANDLE_LIMIT 9

#define FLAG_INDEX 2

#define INIT_FLAG 1
#define INIT_OK 2
#define INIT_ERR 3
#define BROADCAST_FLAG 4
#define MESSAGE_FLAG 5
#define MESSAGE_ERR 7
#define EXIT_FLAG 8
#define EXIT_OK 9
#define LIST_FLAG 10
#define LIST_NUMBER 11
#define LIST_HANDLE 12
#define LIST_DONE 13

typedef struct handle *handle;
//WAKEUP WILLIAM
struct chat_header
{
    unsigned short pdu_len;
    unsigned char flag;
}__attribute__((packed)); 


struct handle
{
    unsigned char len;
    unsigned char field[MAX_SIZE];
}__attribute__((packed));

//use a global variable to count the pdulen
void parseMessage(unsigned char *packetBuf, int packetLen);
void parseMessageErr(unsigned char *packetBuf, int packetLen);
void sendPacket4(int socketNum, unsigned char *message, unsigned char *name);
void sendHandlePacket(int socketNum, unsigned char *name, int handleLen, int flag);
int processPacket1(int socketNum, unsigned char *field, int handleLen);

void processPacket4(unsigned char *packetBuf, int packetLen);
void processPacket5(int socketNum, unsigned char *packetBuf, int packetLen);
void processPacket8(int socketNum);
void processPacket10(int socketNum);
void processPacket11(int socketNum, unsigned char *packetBuf);

int processServerPacket(int, unsigned char *, int);
void initHandle(handle hdl);
void initTable();
int checkTable(unsigned char *handle, int handleLen);
void addTable(int socketNumber, unsigned char *field, int handleLen);
void delTable(int socketNumber);
void sendChatHeader(int socketNum, int flag);
int processServerPacket(int socketNum, unsigned char *packetBuf, int packetLen);
int recvPacket(int clientSocket, unsigned char *buf);
#endif