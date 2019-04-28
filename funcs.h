#ifndef FUNCS_H
#define FUNCS_H

#define MAX_SIZE 100
#define HANDLE_LIMIT 9

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
int processServerPacket(int, unsigned char *, int);
#endif