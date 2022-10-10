// Link layer protocol implementation

#include "link_layer.h"



#define O_RDWR          0x0002          /* open for reading and writing */
#define O_NOCTTY        0x00020000      /* don't assign controlling terminal */


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



// MISC
#define _POSIX_SOURCE 1 // POSIX compliant source

////////////////////////////////////////////////
// LLOPEN
////////////////////////////////////////////////
int llopen(LinkLayer connectionParameters)
{
    int fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY);

    if (fd < 0)
    {
        perror(connectionParameters.serialPort);
        exit(-1);
    }


    // utilizar resto dos parametros do LinkLayer

    if (connectionParameters.role) { // if recetor
        return 1;
    }

    if (!connectionParameters.role) { // if emissor

        // Create string to send
        unsigned char SET[5];
        unsigned char buf[BUF_SIZE + 1] = {0};
    
        SET[0] = FLAG;
        SET[1] = A;
        SET[2] = C_SET;
        SET[3] = BCC_SET;
        SET[4] = FLAG;

        while(!UA_RCV){

                printf("aqui1\n");
                
                state = START;
                int bytes = write(fd, SET, 5);
                printf("%d bytes written\n", bytes);
                // Wait until all bytes have been written to the serial port
                sleep(1);

                alarm(3);
                alarmEnabled = TRUE;
                

                while (state != STOP_S) {
                printf("state: %d\n", (int)state);
                bytes = read(fd, buf, 1);
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

        printf("success ua received\n");


    }


    return 1;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(const unsigned char *buf, int bufSize)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(unsigned char *packet)
{
    // TODO

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int showStatistics)
{
    // TODO

    return 1;
}
