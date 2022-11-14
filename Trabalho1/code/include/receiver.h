#ifndef _RECEIVER_H_
#define _RECEIVER_H_

#include <fcntl.h>
#include "link_layer.h"
#include <string.h>
#include <stdio.h>

int receiver(int fd);

int readPacket(unsigned char * packet);

int readControlPacket(unsigned char * packet, char* filename, int* filesize);

int checkSequenceNr(int number);

#endif // _RECEIVER_H_