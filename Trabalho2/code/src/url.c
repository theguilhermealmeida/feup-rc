#include "url.h"

int getIp(char* adress,URL *url){
    struct hostent *h;

    if ((h = gethostbyname(adress)) == NULL) {
        printf("Error getting host by name!\n");
        return -1;
    }
    strcpy(url->host_name,h->h_name);
    strcpy(url->ip,inet_ntoa(*((struct in_addr *) h->h_addr)));
    return 0;
}

int parseUrl(char* text, URL *url){
    char* ftp = strtok(text,"/");  //ftp    
    char* usrpasshost = strtok(NULL,"/"); //[<user>:<password>@]<host>
    char* path = strtok(NULL,""); //<url-path>
    strcpy(url->path,path);

    if(strcmp(ftp,"ftp:")!=0){
        printf("Error: not using ftp!\n");
        return 1;
    }

    char* user = strtok(usrpasshost,":");
    char* pass = strtok(NULL,"@");

    if (pass == NULL ){
        user = "anonymous";
        pass = "pass";
        strcpy(url->host,usrpasshost);
    }
    else{
        strcpy(url->host,strtok(NULL,""));
    }

    strcpy(url->user,user);
    strcpy(url->password,pass);


    if(getIp(url->host,url)!=0){
        printf("Error getting ip\n");
        return -1;
    }


    char fullpath[128];
    strcpy(fullpath,url->path); //to not change path
    char* tkn = strtok(fullpath,"/");
    while(tkn!=NULL){
        strcpy(url->filename,tkn);
        tkn = strtok(NULL,"/");
    }

    return 0;
}