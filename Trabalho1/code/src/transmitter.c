#include "transmitter.h"
#include "utils.h"


int transmitter(int fd, const char *filename)
{
    struct stat file_stat;
    int file_fd;

    // Reads file info using stat
    if (stat(filename, &file_stat) < 0)
    {
        printf("Error getting file size.");
    }

    // Opens file to transmit
    if ((file_fd = open(filename, O_RDONLY)) < 0)
    {
        printf("Error opening file.");
    }


    sendControlPacket(fd, 2, file_stat.st_size, filename);

    unsigned char buffer[PACKET_SIZE];
    int bytes;
    int sequenceNr = 0;
    float writtenBytes = 0;

    while ((bytes = read(file_fd, buffer, PACKET_SIZE-4)) > 0){
        sendDataPacket(fd,sequenceNr,bytes,buffer);
	
	writtenBytes += bytes;
	printProgressBar(writtenBytes, file_stat.st_size);
        sequenceNr++;
    }

    sendControlPacket(fd, 3, file_stat.st_size, filename);

    if (close(file_fd) < 0)
    {
        printf("Error closing file. ");
        return -1;
    }

    return 0;
}

int sendControlPacket(int fd, unsigned char ctrl_field, unsigned file_size, const char *filename)
{
    unsigned L1 = sizeof(file_size);
    unsigned L2 = strlen(filename);
    unsigned packet_size = 5 + L1 + L2;


    unsigned char packet[packet_size];
    packet[0] = ctrl_field;
    packet[1] = 0;
    packet[2] = L1;
    memcpy(&packet[3], &file_size, L1);
    packet[3 + L1] = 1;
    packet[4 + L1] = L2;
    memcpy(&packet[5 + L1], filename, L2);

    return llwrite(fd, packet, packet_size);
}

int sendDataPacket(int fd, int sequenceNr , int size, unsigned char * buffer){
    unsigned char packet[size];
    packet[0] = 1;
    packet[1] = sequenceNr % 255;
    packet[2] = size / 256;
    packet[3] = size % 256;
    memcpy(&packet[4], buffer, size);

    return llwrite(fd,packet,size + 4);
}
