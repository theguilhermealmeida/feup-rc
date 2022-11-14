#ifndef _URL_H_
#define _URL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct{
    char user[128];
    char password[128];
    char host[128];
    char path[128];
    char ip[128];
    char filename[256];
    char host_name[128];
} URL;

int parseUrl(char* text, URL *url);

int getIp(char* adress,URL *url);

#endif