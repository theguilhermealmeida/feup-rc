// Utils header.

#ifndef _UTIL_H_
#define _UTIL_H_

int byte_stuffing(unsigned char *buf, int bufSize);

int byte_destuffing(unsigned char *buf, int bufSize);

int createBCC(const unsigned char *src, unsigned char *newBuff,int bufSize);


#endif // _UTIL_H_
