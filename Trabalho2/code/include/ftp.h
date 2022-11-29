#ifndef _FTP_H_
#define _FTP_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int startSocket(int * sockfd, char* ip, int port);

int readResponse(FILE* socketResponse ,char* response,size_t size);
int readIp_Port(FILE* socketResponse ,char* response,size_t size,char* ip,int * port);
int sendCommand(int sockfd, char* command);
int saveFile(char * filename, int sockfd);

#endif