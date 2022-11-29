#include "download.h"

int download(char* ftp_link){

    URL url;
    char urlcpy[256];
    char command[256];
    int sockfd;
    int sockfd_b;

    strcpy(urlcpy,ftp_link);
    
    if(parseUrl(urlcpy,&url)!=0){
        printf("Error parsing URL\n");
        return -1;
    }

    printf("user:%s\npassword:%s\nhost:%s\npath:%s\nip:%s\nfilename:%s\nhost_name:%s\n",url.user,url.password,url.host,url.path,url.ip,url.filename,url.host_name);

    if(startSocket(&sockfd,url.ip,21) !=0){
        printf("Error starting socket\n");
        return -1;
    }

    FILE* socketResponse = fdopen(sockfd,"r");
    char response[512];

    readResponse(socketResponse,response,512);

    sprintf(command, "user %s\n",url.user); //pode faltar \r em windows
    sendCommand(sockfd,command);
    readResponse(socketResponse,response,512);
    sprintf(command, "pass %s\n",url.password);
    sendCommand(sockfd,command);
    readResponse(socketResponse,response,512);

    char ip[32];
    int port;

    sprintf(command, "pasv\n");
    sendCommand(sockfd,command);
    readIp_Port(socketResponse,response,512,ip,&port);

    printf("ip: %s    port: %d\n",ip,port);

    if(startSocket(&sockfd_b,ip,port) !=0){
        printf("Error starting socket\n");
        return -1;
    }

    sprintf(command,"retr %s\n",url.path);
    sendCommand(sockfd,command);
    readResponse(socketResponse,response,512);

    saveFile(url.filename,sockfd_b);

    return 0;

}