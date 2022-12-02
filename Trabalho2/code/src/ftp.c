#include "ftp.h"

int startSocket(int * sockfd, char* ip, int port){

    struct sockaddr_in server_addr;

    /*server address handling*/
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);    /*32 bit Internet address network byte ordered*/
    server_addr.sin_port = htons(port);        /*server TCP port must be network byte ordered */

    /*open a TCP socket*/
    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error opening TCP socket!\n");
        return -1;
    }
    /*connect to the server*/
    if (connect(*sockfd,
                (struct sockaddr *) &server_addr,
                sizeof(server_addr)) < 0) {
        printf("Error connecting to the server!\n");
        return -1;
    }

    return 0;
}

int readResponse(FILE* socketResponse ,char* response,size_t size){

    while(fgets(response,size,socketResponse)){
        printf("%s",response);
        if(response[3] == ' '){
            int code = atoi(response);
            printf("command %d\n",code);
            if (code == 550 || code == 530)
            {
                printf("Command error\n");
                return -1;
            }
            break;
        }
    }
    return 0;
}

int readIp_Port(FILE* socketResponse ,char* response,size_t size,char* ip,int * port){

    while(fgets(response,size,socketResponse)){
        printf("%s",response);
        if(response[3] == ' '){
            break;
        }
    }

    strtok(response,"(");
    char* ip_1 = strtok(NULL, ",");
    char* ip_2 = strtok(NULL, ",");
    char* ip_3 = strtok(NULL, ",");
    char* ip_4 = strtok(NULL, ",");
    char* port_1 = strtok(NULL, ",");
    char* port_2 = strtok(NULL, ")");

    sprintf(ip,"%s.%s.%s.%s",ip_1,ip_2,ip_3,ip_4);
    *port = atoi(port_1)*256 + atoi(port_2); 

    return 0;
}

int sendCommand(int sockfd, char* command){

    size_t bytes = write(sockfd, command, strlen(command));
    if (bytes > 0)
        printf("Bytes escritos %ld\n", bytes);
    else {
        perror("write()");
        return -1;
    }

    return 0;
}

int saveFile(char * filename, int sockfd){
    int filefd = open(filename,O_WRONLY| O_CREAT,0777);
    if(filefd < 0){
        perror("open file");
        return -1;
    }

    size_t bytes;
    char buffer[512];

    do{
        bytes = read(sockfd,buffer,512);
        if(bytes > 0){
            write(filefd,buffer,bytes);
        }
    }while(bytes!=0);

    close(filefd);

    return 0;
}