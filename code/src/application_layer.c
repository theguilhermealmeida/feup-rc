// Application layer protocol implementation
#include "application_layer.h"
#include <sys/time.h>

#define _POSIX_SOURCE 1 // POSIX compliant source
#define FALSE 0
#define TRUE 1

LinkLayer connectionParameters;

int fd;
int finish = FALSE;
char *FileName; 

void printStatistics(int role);
void applicationLayer(const char *serialPort, const char *role, int baudRate,

                      int nTries, int timeout, const char *filename)

{
    struct timeval r_start, t_start, r_end, t_end;
    FileName = malloc(sizeof(filename));

    strcpy(FileName, filename);
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
	gettimeofday(&r_start , NULL);

        while (!finish)
        {
		receiver(fd);
        }

	printStatistics(0);
        gettimeofday(&r_end, NULL);
        double time_spent = (r_end.tv_sec - r_start.tv_sec) * 1e6;
        time_spent = (time_spent +(r_end.tv_usec - r_start.tv_usec)) * 1e-6;
	printf("Time spent: %f seconds\n", time_spent);
        printf("\nFINISHING PROGRAM \n");
    }

    if (!connectionParameters.role)
    { // if emissor
	gettimeofday(&t_start , NULL);

        transmitter(fd,filename);
        llclose(fd);

        // unsigned char *string1 = (unsigned char *)"a~~oz";
        // unsigned char *string2 = (unsigned char *)"a}}oz";
        // unsigned char *string3 = (unsigned char *)">@";
        // unsigned char *string4 = (unsigned char *)"O tiago e mau";
        // unsigned char *string5 = (unsigned char *)"O joao e feio";
        // llwrite(fd, string1, 6);
        // llwrite(fd, string2, 6);
        // llwrite(fd, string3, 3);
        // llwrite(fd, string4, 14);
        // llwrite(fd, string5, 14);

	printStatistics(1);
        gettimeofday(&t_end, NULL);
        double time_spent = (t_end.tv_sec - t_start.tv_sec) * 1e6;
        time_spent = (time_spent +(t_end.tv_usec - t_start.tv_usec)) * 1e-6;
	printf("Time spent: %f seconds\n", time_spent);
    }
}

