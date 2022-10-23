#include "utils.h"
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>

int createBCC(const unsigned char *src, unsigned char *newBuff, int bufSize)
{
    unsigned char BCC2 = 0;
    for (int i = 0; i < bufSize; i++)
    {
        newBuff[i] = src[i];
        BCC2 ^= src[i];
    }
    newBuff[bufSize] = BCC2;
    //newBuff[bufSize] = 0;

    return 0;
}

int byte_stuffing(unsigned char *buf, int bufSize)
{
    int newBufSize = 0;
    unsigned char newBuff[bufSize * 2];
    for (int i = 0; i < bufSize; i++)
    {
        if (buf[i] == 0x7E)
        {
            newBuff[newBufSize] = 0X7D;
            newBufSize++;
            newBuff[newBufSize] = 0X5E;
            newBufSize++;
        }
        else if (buf[i] == 0x7D)
        {
            newBuff[newBufSize] = 0X7D;
            newBufSize++;
            newBuff[newBufSize] = 0X5D;
            newBufSize++;
        }
        else
        {
            newBuff[newBufSize] = buf[i];
            newBufSize++;
        }
    }
    memcpy(buf, newBuff, newBufSize);
    return newBufSize;
}

int byte_destuffing(unsigned char *buf, int bufSize)
{
    int newBufSize = 0;
    unsigned char newBuff[bufSize * 2];
    for (int i = 0; i < bufSize; i++)
    {
        if (buf[i] == 0x7D)
        {
            if (buf[i + 1] == 0x5E)
            {
                newBuff[newBufSize] = 0X7E;
                newBufSize++;
                i++;
            }
            else if (buf[i+1] == 0x5D)
            {
                newBuff[newBufSize] = 0X7D;
                newBufSize++;
                i++;
            }
        }
        else
        {
            newBuff[newBufSize] = buf[i];
            newBufSize++;
        }
    }
    memcpy(buf, newBuff, newBufSize);
    return newBufSize;
}

char * getFilename(char * path) {
    char * filename = path, *p;
    for (p = path; *p; p++) {
        if (*p == '/' || *p == '\\' || *p == ':') {
            filename = p;
        }
    }
    return filename;
}

int checkFileSize(int filesize, const char* filename){

    struct stat file_stat;

    if (stat(filename, &file_stat) < 0){
        printf("Error getting file information.");
        return -1;
    }

    return (filesize != file_stat.st_size);
}

void printProgressBar(float current, float total)  {

	float percentage = 100.0 * current / total;

	printf("\n%.2f%% [", percentage);
	
	int len = 50;
	int pos = percentage * len / 100.0;
	for (int i = 0; i < len; i++) 
		i <= pos ? printf("#") : printf(" ");

	printf("]\n");	
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void printStatistics(int role)  {

	if (!role) {
		printf("\nSTATISTICS\n");
		printf("Number of  RR sent: %d\n", 0);
		printf("Number of  REJ sent: %d\n", 0);
	}

	if (role) {
		printf("\nSTATISTICS\n\n");
		printf("Number of  RR received: %d\n", 0);
		printf("Number of  REJ received: %d\n", 0);
	}
}

