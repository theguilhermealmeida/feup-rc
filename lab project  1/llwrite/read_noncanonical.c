// Read from serial port in non-canonical mode
//
// Modified by: Eduardo Nuno Almeida [enalmeida@fe.up.pt]

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define BCC_SET A^C_SET

#define C_UA 0x07
#define BCC_UA A^C_UA

volatile int STOP = FALSE;

typedef enum {
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    DATA_RCV,
    STOP_S
} STATE;

STATE state = START;

int main(int argc, char *argv[])
{
    // Program usage: Uses either COM1 or COM2
    const char *serialPortName = argv[1];

    if (argc < 2)
    {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

        // printf("New termios structure set\n");

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0; // Inter-character timer unused
    newtio.c_cc[VMIN] = 0;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    // Loop for input
    unsigned char buf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
    unsigned char buf2[BUF_SIZE + 1] = {0};
    unsigned char data[500] = {0};
    int count_data = 0;

    int bytes;
    
    unsigned char C[1];
    

    while (state != STOP_S) {
        
        bytes = read(fd, buf, 1);

        if (bytes > 0) {
             printf("state: %d\n", (int)state);
             printf("%d\n", buf[0]);
            switch(state) {
                case START:
                    if (buf[0] == FLAG) {
                        state = FLAG_RCV;   
                        }
                    break;

                case FLAG_RCV:
                    if (buf[0] == A) {
                       state = A_RCV;
                       }
                    else if (buf[0] == FLAG) {
                       state = FLAG_RCV;
                        }
                    else {state = START;}

                    break;

                case A_RCV:
                    if (buf[0] == 0) {
                       state = C_RCV;
                       C[0] = 0;
                       }
                    else if (buf[0] == FLAG) {
                       state = FLAG_RCV;
                        }
                    else {state = START;}


                    break;

                case C_RCV:
                    if (buf[0] == (A^C[0])) {
                       state = BCC_OK;
                       }
                    else if (buf[0] == FLAG) {
                        state = FLAG_RCV;
                        }
                    else { state = START;}

                    break;

                case BCC_OK:
                    if (buf[0] == FLAG) {
                        state = FLAG_RCV;
                    }
                    else {
                        data[count_data] = buf[0];
                        count_data++;
                        state = DATA_RCV;}
                    
                    break;
               
                case DATA_RCV:
                    if (buf[0] == FLAG) {
                        state = STOP_S;
                    }
                    else {
                        data[count_data] = buf[0];
                        count_data++;
                    }
                    
                    
                    
                    break;
           }
        }
    }

    for (int i = 0; i < count_data; i++) {
        printf("%d\n", data[i]);
    }
    
    unsigned char BCC2 = 0;
    
    for (int i = 0; i < count_data-1; i++) {
        BCC2 = BCC2^data[i];
       
    }
    
    printf("BCC2: %d\n", BCC2);
    
    if (BCC2 == data[count_data-1]) {
        printf("BCC2 ok!");
    }
    else {
        printf("BCC2 FAILED!");
    }
       
   
    return 0;
    
  
   unsigned char UA[5];
   
   UA[0] = FLAG;
   UA[1] = A;
   UA[2] = C_UA;
   UA[3] = BCC_UA;
   UA[4] = FLAG;
   /*
   while (1) {
    
    bytes = read(fd, buf, 1);
    if (bytes > 0) {
        printf("%d\n", buf[0]);
    }
   }
   */
   bytes = write(fd,UA,5);
   
   


    // The while() cycle should be changed in order to respect the specifications
    // of the protocol indicated in the Lab guide

    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}
