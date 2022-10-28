// Utils header.

#ifndef _UTIL_H_
#define _UTIL_H_

#include <unistd.h>

#define PACKET_SIZE 2500

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define BCC_SET A ^ C_SET
#define C_UA 0x07
#define BCC_UA A ^ C_UA
#define C_RR 0x05
#define C_DISC 0x0B
#define BCC_DISC A ^ C_DISC
#define C_REJ 0X01


int byte_stuffing(unsigned char *buf, int bufSize);

int byte_destuffing(unsigned char *buf, int bufSize);

void createBCC(const unsigned char *src, unsigned char *newBuff,int bufSize);

int sendFrame(int fd, unsigned char C, unsigned char BCC);

int sendInformationFrame(int fd, unsigned char C, unsigned char BCC,const unsigned char *buf, int bufSize);

int checkFileSize(int filesize, const char* filename);

void printProgressBar(float current, float total);

void printStatistics(int role, int RR , int REJ);


#endif // _UTIL_H_
