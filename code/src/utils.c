#include "utils.h"
#include <string.h>

void createBCC(const unsigned char *src, unsigned char *newBuff, int bufSize)
{
    unsigned char BCC2 = 0;
    for (int i = 0; i < bufSize; i++)
    {
        newBuff[i] = src[i];
        BCC2 ^= src[i];
    }
    newBuff[bufSize] = BCC2;
    //newBuff[bufSize] = 0;

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
            else if (buf[i + 1] == 0x5D)
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

char *getFilename(char *path)
{
    char *filename = path, *p;
    for (p = path; *p; p++)
    {
        if (*p == '/' || *p == '\\' || *p == ':')
        {
            filename = p;
        }
    }
    return filename;
}

int sendFrame(int fd,unsigned char C, unsigned char BCC)
{
    unsigned char FRAME[5];

    FRAME[0] = FLAG;
    FRAME[1] = A;
    FRAME[2] = C;
    FRAME[3] = BCC;
    FRAME[4] = FLAG;

    return write(fd, FRAME, 5);
}

int sendInformationFrame(int fd, unsigned char C, unsigned char BCC,const unsigned char *buf, int bufSize){

    unsigned char FRAME[1000];
    int buf_cnt = 4;

    FRAME[0] = FLAG;
    FRAME[1] = A;
    FRAME[2] = C;
    FRAME[3] = BCC;

    for (int i = 0; i < bufSize; i++)
    {
        FRAME[buf_cnt] = buf[i];
        buf_cnt++;
    }

    FRAME[buf_cnt] = FLAG;

    return write(fd, FRAME, buf_cnt + 2);
}