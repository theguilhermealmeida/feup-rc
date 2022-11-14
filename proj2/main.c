#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include <string.h>

#include "utils.h"

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s ftp://[<user>:<password>@]<host>/<url-path>\n", argv[0]);
        return -1;
    }

    url url;

    if (parseURL(&url, argv[1])) return -1;

    printf(" - Username: %s\n", url.user);
	printf(" - Password: %s\n", url.password);
	printf(" - Host: %s\n", url.host);
	printf(" - Path: %s\n", url.path);
	printf(" - Filename: %s\n", url.filename);



    return 0;
}




