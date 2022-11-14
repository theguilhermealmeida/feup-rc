#include "download.h"

int download(char* ftp_link){

    URL url;
    char urlcpy[256];

    strcpy(urlcpy,ftp_link);
    
    if(parseUrl(urlcpy,&url)!=0){
        printf("Error parsing URL\n");
        return -1;
    }

    printf("user:%s\npassword:%s\nhost:%s\npath:%s\nip:%s\nfilename:%s\nhost_name:%s\n",url.user,url.password,url.host,url.path,url.ip,url.filename,url.host_name);


    return 0;

}