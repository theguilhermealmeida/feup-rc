#ifndef _TRANSMITTER_H_
#define _TRANSMITTER_H_

#include <string.h>
#include <stdio.h>
#include "link_layer.h"

int transmitter(int fd, const char* filename);

int sendControlPacket(int fd, unsigned char ctrl_field, unsigned file_size, const char *file_name);

int sendDataPacket(int fd,int sequenceNr , int size, unsigned char * buffer);

#endif // _TRANSMITTER_H_