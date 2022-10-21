// Application layer protocol implementation
#include "application_layer.h"

#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

LinkLayer connectionParameters;

int fd;
int finish = FALSE;

void applicationLayer(const char *serialPort, const char *role, int baudRate,

                      int nTries, int timeout, const char *filename)

{
    strcpy(connectionParameters.serialPort, serialPort);
    if (strcmp(role, "tx") == 0)
    {
        connectionParameters.role = LlTx;
    }
    else if (strcmp(role, "rx") == 0)
    {
        connectionParameters.role = LlRx;
    }
    connectionParameters.baudRate = baudRate;
    connectionParameters.nRetransmissions = nTries;
    connectionParameters.timeout = timeout;

    fd = llopen(connectionParameters);

    if (connectionParameters.role)
    { // if recetor
        while (!finish)
        {
            unsigned char packet[1000] = {0};
            llread(fd, packet);
            printf("packet :%s\n", packet);
            //readPacket(packet);
        }
        printf("FINISHING PROGRAM \n");
    }

    if (!connectionParameters.role)
    { // if emissor
        //transmitter(fd,filename);
        unsigned char *string1 = (unsigned char *)"a~~oz";
        unsigned char *string2 = (unsigned char *)"a}}oz";
        unsigned char *string3 = (unsigned char *)">@";
        unsigned char *string4 = (unsigned char *)"O tiago e mau";
        unsigned char *string5 = (unsigned char *)"O joao e feio";
        llwrite(fd, string1, 6);
        llwrite(fd, string2, 6);
        llwrite(fd, string3, 3);
        llwrite(fd, string4, 14);
        llwrite(fd, string5, 14);
        llclose(fd);

    }
}

