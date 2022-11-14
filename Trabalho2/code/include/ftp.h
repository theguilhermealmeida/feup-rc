#ifndef _FTP_H_
#define _FTP_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>

int startSocket(int * sockfd, char* ip, int port);

#endif