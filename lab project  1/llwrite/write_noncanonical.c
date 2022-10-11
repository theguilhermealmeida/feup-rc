// Write to serial port in non-canonical mode
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
#include <signal.h>

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
    STOP_S
} STATE;

STATE state = START;

int alarmEnabled = FALSE;
int alarmCount = 0;

int UA_RCV = FALSE;

// Alarm function handler
void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;
    state = STOP_S;

    printf("Alarm #%d\n", alarmCount);
}

int main(int argc, char *argv[])
{

    // Set alarm function handler
    (void)signal(SIGALRM, alarmHandler);

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

    // Open serial port device for reading and writing, and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);

    if (fd < 0)
    {
        perror(serialPortName);
        exit(-1);
    }

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

    // Create string to send
    unsigned char I[5];
    unsigned char buf[BUF_SIZE + 1] = {0};
   
    I[0] = FLAG;
    I[1] = A;
    I[2] = 0X00;
    I[3] = I[1] ^ I[2]; 
    I[4] = 0X01;
    I[5] = 0X02;
    I[6] = 0X03;
    I[7] = I[3] ^ I[4] ^ I[5];
    I[8] = FLAG;

    while(!UA_RCV){

        printf("aqui1\n");
        
        state = START;
        int bytes = write(fd, I, 9);
        printf("%d bytes written\n", bytes);
        // Wait until all bytes have been written to the serial port
        sleep(1);

        return 0;
        
        if(alarmCount >= 3){
            printf("Maximum tries reached. Ending program");
            break;                   
        }

        alarm(3);
        alarmEnabled = TRUE;

 

        while (state != STOP_S) {
        printf("state: %d\n", (int)state);
        bytes = read(fd, buf, 1);
        printf("BYTES: %d\n", bytes);
        if (bytes > 0){
            printf("%u\n", buf[0]);
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
                    if (buf[0] == C_UA) {
                       state = C_RCV;
                       }
                    else if (buf[0] == FLAG) {
                       state = FLAG_RCV;
                        }
                    else {state = START;}


                    break;

                case C_RCV:
                    if (buf[0] == (BCC_UA)) {
                       state = BCC_OK;
                       }
                    else if (buf[0] == FLAG) {
                        state = FLAG_RCV;
                        }
                    else { state = START;}

                    break;

                case BCC_OK:
                    //printf("entrou");
                    state = STOP_S;
                    UA_RCV = TRUE;
            }
        }
    }

    }
    
    if(UA_RCV){
        printf("success ua received\n");
    }


    // Restore the old port settings
    if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    close(fd);

    return 0;
}
