// Application layer protocol implementation
#include "application_layer.h"
#include "link_layer.h"
#include <string.h>
#include <stdio.h>


#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

LinkLayer connectionParameters;

int fd;


void applicationLayer(const char *serialPort, const char *role, int baudRate,

                      int nTries, int timeout, const char *filename)

{
    strcpy(connectionParameters.serialPort, serialPort);
    if(strcmp(role, "tx") == 0){
        connectionParameters.role = LlTx;
    }
    else if (strcmp(role, "rx") == 0){
        connectionParameters.role = LlRx;
    }
    connectionParameters.baudRate = baudRate;
    connectionParameters.nRetransmissions = nTries;
    connectionParameters.timeout = timeout; 

    unsigned char packet[500] = {0};

    fd = llopen(connectionParameters);

    if (connectionParameters.role) { // if recetor
        llread(fd,packet);
        printf("packet :%s\n",packet);
        memset(packet,0,500);
        llread(fd,packet);
        printf("packet :%s\n",packet);
        
        memset(packet,0,500);
        llread(fd,packet);
        printf("packet :%s\n",packet);
        memset(packet,0,500);
        llread(fd,packet);
        printf("packet :%s\n",packet);
        memset(packet,0,500);
        llread(fd,packet);
        printf("packet :%s\n",packet);
        llread(fd,packet);
        
    }

    if (!connectionParameters.role) { // if emissor
        unsigned char *string1 = (unsigned char *)"O tiago e lindzzzzzzzz";
        unsigned char *string2 = (unsigned char *)"O tiago e feio";
        unsigned char *string3 = (unsigned char *)"O tiago e bom";
        unsigned char *string4 = (unsigned char *)"O tiago e mau";
        unsigned char *string5 = (unsigned char *)"O joao e feio";
        llwrite(fd,string1,22);
        llwrite(fd,string2,14);
        llwrite(fd,string3,13);
        llwrite(fd,string4,13);
        llwrite(fd,string5,13);
        llclose(fd);
    }
}
