#ifndef FUNCS_H
#define FUNCS_H

#define MAX_SIZE 100
#define HANDLE_LIMIT 9


struct chat_header
{
    unsigned short pdu_len;
    unsigned char flag;
};

struct handle
{
    unsigned char len;
    char field[100];
};

//use a global variable to count the pdulen
int process_packet(char *, int);
#endif