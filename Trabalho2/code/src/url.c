#include "url.h"

int getIp(char* adress,URL *url){
    struct hostent *h;

    if ((h = gethostbyname(adress)) == NULL) {
        printf("Error getting host by name!\n");
        return -1;
    }

    strcpy(url->ip,inet_ntoa(*((struct in_addr *) h->h_addr)));
    strcpy(url->host_name,h->h_name);
    
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
        pass = "something";
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

    //getting filename
    char* ptr = strrchr(url->path, '/');
    ptr++; // ignoring '/'
    strcpy(url->filename, ptr);

    return 0;
}