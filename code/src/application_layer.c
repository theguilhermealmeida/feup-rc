// Application layer protocol implementation
#include "application_layer.h"
#include "link_layer.h"
#include <string.h>
#include <stdio.h>


#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

LinkLayer connectionParameters;

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

    llopen(connectionParameters);

    if (connectionParameters.role) { // if recetor
        llread(packet);
        printf("packet :%s",packet);
    }

    if (!connectionParameters.role) { // if emissor
        unsigned char *string = (unsigned char *)"O tiago e lindo";
        llwrite(string,15);
    }

    llclose(0);
}
