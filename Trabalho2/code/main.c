#include "download.h"


int main(int argc, char** argv){
    if(argc != 2){
        printf("usage : download ftp://[<user>:<password>@]<host>/<url-path>");
        return -1;
    }

    download(argv[1]);

    return 0;

}