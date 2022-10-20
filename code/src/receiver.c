#include "receiver.h"

int receiver(int fd){
    printf("AQUI1");
    unsigned char packet[1000] = {0};
    llread(fd, packet);
    printf("AQUI2");
    readPacket(packet);
    printf("AQUI3");
    return 0;
}

int readPacket(unsigned char * packet){
    static int newFile;
    static int filesize = 0;
    char filename[30];

    if (packet[0] == 2){
        readControlPacket(packet,filename,&filesize);
        if ((newFile = open(filename, O_WRONLY | O_CREAT, 0777)) < 0) {
            printf("Error opening new file!");
            return -1;
        }
    }
    else if (packet[0] == 3){
        if (close(newFile) < 0) {
            printf("Error closing file!");
            return -1;
        }
    }
    else if (packet[0] == 1){
        if (checkSequenceNr(packet[1]) != 0) {
            printf("Error in sequence number\n");
            return -1;
        }
        int datasize = packet[3] + 256 * packet[2];
        if (write(newFile, &packet[4], datasize) < 0) {
            printf("Error writing to file!");
            return -1;
        }
    }
    return 0;
}

int readControlPacket(unsigned char * packet, char* filename, int* filesize){
    printf("control\n");
    unsigned L1 = packet[2];
    if(packet[1] == 0){
        for(int i = 0 ; i < L1;i++){
            *filesize |= (packet[3+i] << (i*8));
        }
    }
    printf("leu filesize : %d\n",*filesize);
    if(packet[L1 + 3] == 1){
        unsigned L2 = packet[L1 + 4];
        char name[L2 + 1];
        for(int i = 0; i < L2;i++){
            name[i] = packet[L1 + 5 + i];
        }
        name[L2] = '\0';
        name[0] = 'A';
        strcpy(filename, name);
        printf("filename : %s\n",filename);
    }
    printf("acabou control\n");
    return 0;
}

int checkSequenceNr(int number) {
    static int sequenceNr = 0;

    if (number == sequenceNr) {
        sequenceNr = (sequenceNr + 1) % 255;
        return 0;
    }
    
    return -1;
}