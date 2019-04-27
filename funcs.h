#ifndef FUNCS_H
#define FUNCS_H

#define NET_CLIENT 0
#define NET_SERVER 1
struct chat_header
{
    unsigned short pdu_len;
    unsigned char flag;
};

//use a global variable to count the pdulen
int process_packet(char *, int);
#endif