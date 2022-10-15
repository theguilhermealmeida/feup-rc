// Link layer protocol implementation

#include "link_layer.h"

#define BUF_SIZE 1

#define FLAG 0x7E
#define A 0x03
#define C_SET 0x03
#define BCC_SET A ^ C_SET
#define C_UA 0x07
#define BCC_UA A ^ C_UA
#define C_RR 0x05
#define C_DISC 0x0B
#define BCC_DISC A ^ C_DISC

volatile int STOP = FALSE;

typedef enum
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    C_DATA_RCV,
    BCC_OK,
    BCC_DATA_OK,
    DATA_RCV,
    STOP_S,
    STOP_DATA
} STATE;

STATE state = START;

int alarmEnabled = FALSE;
int alarmCount = 0;
int ns;
int nr;

struct termios oldtio;
struct termios newtio;

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
    state = START;
    int UA_RCV = FALSE;
    // Set alarm function handler
    (void)signal(SIGALRM, alarmHandler);

    int fd = open(connectionParameters.serialPort, O_RDWR | O_NOCTTY);

    if (fd < 0)
    {
        perror(connectionParameters.serialPort);
        exit(-1);
    }

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

    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 0;

    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }
    printf("New termios structure set\n");

    // utilizar resto dos parametros do LinkLayer

    if (connectionParameters.role) // if recetor
    {
        nr = 1;
        unsigned char buf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char

        int bytes;
        while (state != STOP_S)
        {

            bytes = read(fd, buf, 1);
            // printf("%d\n", buf[0]);
            if (bytes > 0)
            {
                switch (state)
                {
                case START:
                    if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    break;

                case FLAG_RCV:
                    if (buf[0] == A)
                    {
                        state = A_RCV;
                    }
                    else if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case A_RCV:
                    if (buf[0] == C_SET)
                    {
                        state = C_RCV;
                    }
                    else if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case C_RCV:
                    if (buf[0] == (BCC_SET))
                    {
                        state = BCC_OK;
                    }
                    else if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case BCC_OK:
                    if (buf[0] == FLAG)
                    {
                        state = STOP_S;
                    }
                    else
                    {
                        state = START;
                    }
                    break;

                default:
                    break;
                }
            }
        }

        printf("success set receive\n");

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
        }*/

        bytes = write(fd, UA, 5);
        printf("sent UA -> %d bytes written\n", bytes);
    }

    if (!connectionParameters.role) // if emissor
    {
        // Create string to send
        ns = 0;
        unsigned char SET[5];
        unsigned char buf[BUF_SIZE + 1] = {0};

        SET[0] = FLAG;
        SET[1] = A;
        SET[2] = C_SET;
        SET[3] = BCC_SET;
        SET[4] = FLAG;

        while (!UA_RCV)
        {
            if (alarmCount >= 3)
            {
                printf("Maximum tries reached. Ending program\n");
                return -1;
            }
            state = START;
            int bytes = write(fd, SET, 5);
            printf("sent SET -> %d bytes written\n", bytes);
            // Wait until all bytes have been written to the serial port
            sleep(1);

            alarm(3);
            alarmEnabled = TRUE;

            while (state != STOP_S)
            {
                bytes = read(fd, buf, 1);
                if (bytes > 0)
                {
                    switch (state)
                    {
                    case START:
                        if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        break;

                    case FLAG_RCV:
                        if (buf[0] == A)
                        {
                            state = A_RCV;
                        }
                        else if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        else
                        {
                            state = START;
                        }

                        break;

                    case A_RCV:
                        if (buf[0] == C_UA)
                        {
                            state = C_RCV;
                        }
                        else if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        else
                        {
                            state = START;
                        }

                        break;

                    case C_RCV:
                        if (buf[0] == (BCC_UA))
                        {
                            state = BCC_OK;
                        }
                        else if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        else
                        {
                            state = START;
                        }

                        break;

                    case BCC_OK:
                        if (buf[0] == FLAG)
                        {
                            state = STOP_S;
                            UA_RCV = TRUE;
                        }
                        else
                        {
                            state = START;
                        }
                        break;

                    default:
                        break;
                    }
                }
            }
        }

        if (UA_RCV)
        {
            printf("success ua received\n");
        }
    }
    return fd;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(int fd, const unsigned char *buf, int bufSize)
{
    unsigned char buffer[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
    int RR_RCV = FALSE;
    alarmCount = 0;
    unsigned char BCC2 = 0;
    int buf_cnt = 4;
    unsigned char I[bufSize + 6];

    I[0] = FLAG;
    I[1] = A;
    I[2] = ns;
    I[3] = I[1] ^ I[2];
    for (int i = 0; i < bufSize; i++)
    {
        I[buf_cnt] = buf[i];
        BCC2 ^= buf[i];
        buf_cnt++;
    }
    I[buf_cnt] = BCC2;
    I[buf_cnt + 1] = FLAG;

    while (!RR_RCV)
    {
        if (alarmCount >= 3)
        {
            printf("Maximum tries reached. Ending program\n");
            return -1;
        }
        state = START;
        int bytes = write(fd, I, bufSize + 6);
        printf("Sent I -> %d bytes written\n", bytes);
        // Wait until all bytes have been written to the serial port
        sleep(1);

        alarm(3);
        alarmEnabled = TRUE;
        while (state != STOP_S)
        {
            bytes = read(fd, buffer, 1);
            if (bytes > 0)
            {
                // printf("state: %d\n", (int)state);
                // printf("%d\n", buffer[0]);
                switch (state)
                {
                case START:
                    if (buffer[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    break;

                case FLAG_RCV:
                    if (buffer[0] == A)
                    {
                        state = A_RCV;
                    }
                    else if (buffer[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case A_RCV:
                    if (buffer[0] == (C_RR ^ ((ns ^ 1) << 7)))
                    {
                        state = C_RCV;
                    }
                    else if (buffer[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case C_RCV:
                    if (buffer[0] == (C_RR ^ ((ns ^ 1) << 7) ^ A))
                    {
                        state = BCC_OK;
                    }
                    else if (buffer[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case BCC_OK:
                    if (buffer[0] == FLAG)
                    {
                        state = STOP_S;
                        RR_RCV = TRUE;
                        ns ^= 1;
                    }
                    else
                    {
                        state = START;
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    if (RR_RCV)
    {
        printf("success rr received\n");
    }

    return 0;
}

////////////////////////////////////////////////
// LLREAD
////////////////////////////////////////////////
int llread(int fd, unsigned char *packet)
{
    state = START;
    int UA_RCV = FALSE;
    alarmCount = 0;
    unsigned char buf[BUF_SIZE + 1] = {0}; // +1: Save space for the final '\0' char
    unsigned char data[500] = {0};
    int count_data = 0;
    int bytes;

    while (state != STOP_S && state != STOP_DATA)
    {
        bytes = read(fd, buf, 1);
        if (bytes > 0)
        {
            // printf("nr: %d\n",nr);
            // printf("state: %d\n", (int)state);
            // printf("%d\n", buf[0]);
            switch (state)
            {
            case START:
                if (buf[0] == FLAG)
                {
                    state = FLAG_RCV;
                }
                break;

            case FLAG_RCV:
                if (buf[0] == A)
                {
                    state = A_RCV;
                }
                else if (buf[0] == FLAG)
                {
                    state = FLAG_RCV;
                }
                else
                {
                    state = START;
                }

                break;

            case A_RCV:
                if (buf[0] == (nr ^ 1))
                {
                    state = C_DATA_RCV;
                }
                else if (buf[0] == C_DISC)
                {
                    state = C_RCV;
                }
                else if (buf[0] == FLAG)
                {
                    state = FLAG_RCV;
                }
                else
                {
                    state = START;
                }

                break;

            case C_DATA_RCV:
                if (buf[0] == (A ^ (nr ^ 1)))
                {
                    state = BCC_DATA_OK;
                }
                else if (buf[0] == FLAG)
                {
                    state = FLAG_RCV;
                }
                else
                {
                    state = START;
                }

                break;

            case C_RCV:
                if (buf[0] == (BCC_DISC))
                {
                    state = BCC_OK;
                }
                else if (buf[0] == FLAG)
                {
                    state = FLAG_RCV;
                }
                else
                {
                    state = START;
                }

                break;

            case BCC_OK:
                if (buf[0] == FLAG)
                {
                    state = STOP_S;
                }
                else
                {
                    state = START;
                }

                break;

            case BCC_DATA_OK:
                if (buf[0] == FLAG)
                {
                    state = FLAG_RCV;
                }
                else
                {
                    data[count_data] = buf[0];
                    count_data++;
                    state = DATA_RCV;
                }

                break;

            case DATA_RCV:
                if (buf[0] == FLAG)
                {
                    state = STOP_DATA;
                }
                else
                {
                    data[count_data] = buf[0];
                    count_data++;
                }
                break;

            default:
                break;
            }
        }
    }

    if (state == STOP_DATA)
    {
        for (int i = 0; i < count_data - 1; i++)
        {
            packet[i] = data[i];
        }

        unsigned char BCC2 = 0;

        for (int i = 0; i < count_data - 1; i++)
        {
            BCC2 = BCC2 ^ data[i];
        }

        if (BCC2 == data[count_data - 1])
        {
            printf("BCC2 ok!\n");

            unsigned char RR[5];

            RR[0] = FLAG;
            RR[1] = A;
            RR[2] = C_RR ^ (nr << 7);
            RR[3] = RR[1] ^ RR[2];
            RR[4] = FLAG;

            /*
            while (1)
            {

                bytes = read(fd, buf, 1);
                if (bytes > 0)
                {
                    printf("%d\n", buf[0]);
                }
            }*/
            nr ^= 1;
            int bytes = write(fd, RR, 5);
            printf("sent RR -> %d bytes written\n", bytes);
            sleep(1);
        }
        else
        {
            printf("BCC2 FAILED!\n");
        }
    }
    else
    { // DISC received

        printf("DISC received\n");
        unsigned char DISC[5];

        DISC[0] = FLAG;
        DISC[1] = A;
        DISC[2] = C_DISC;
        DISC[3] = BCC_DISC;
        DISC[4] = FLAG;

        /*
        while (1)
        {
            bytes = read(fd, buf, 1);
            if (bytes > 0)
            {
                printf("%d\n", buf[0]);
            }
        }*/

        while (!UA_RCV)
        {
            if (alarmCount >= 3)
            {
                printf("Maximum tries reached. Ending program\n");
                return -1;
            }
            state = START;
            int bytes = write(fd, DISC, 5);
            printf("sent DISC -> %d bytes written\n", bytes);
            // Wait until all bytes have been written to the serial port
            sleep(1);

            alarm(3);
            alarmEnabled = TRUE;

            while (state != STOP_S)
            {
                bytes = read(fd, buf, 1);
                if (bytes > 0)
                {
                    switch (state)
                    {
                    case START:
                        if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        break;

                    case FLAG_RCV:
                        if (buf[0] == A)
                        {
                            state = A_RCV;
                        }
                        else if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        else
                        {
                            state = START;
                        }

                        break;

                    case A_RCV:
                        if (buf[0] == C_UA)
                        {
                            state = C_RCV;
                        }
                        else if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        else
                        {
                            state = START;
                        }

                        break;

                    case C_RCV:
                        if (buf[0] == (BCC_UA))
                        {
                            state = BCC_OK;
                        }
                        else if (buf[0] == FLAG)
                        {
                            state = FLAG_RCV;
                        }
                        else
                        {
                            state = START;
                        }

                        break;

                    case BCC_OK:
                        if (buf[0] == FLAG)
                        {
                            state = STOP_S;
                            UA_RCV = TRUE;
                        }
                        else
                        {
                            state = START;
                        }
                        break;

                    default:
                        break;
                    }
                }
            }
        }

        if (UA_RCV)
        {
            printf("success ua received\n");

            if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
            {
                perror("tcsetattr");
                exit(-1);
            }

            close(fd);
        }
    }

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int fd)
{
    unsigned char DISC[5];
    unsigned char buf[BUF_SIZE + 1] = {0};
    alarmCount = 0;
    int DISC_RCV = FALSE;
    int bytes;
    alarmCount = 0;

    DISC[0] = FLAG;
    DISC[1] = A;
    DISC[2] = C_DISC;
    DISC[3] = BCC_DISC;
    DISC[4] = FLAG;
    while (!DISC_RCV)
    {
        if (alarmCount >= 3)
        {
            printf("Maximum tries reached. Ending program\n");
            return -1;
        }
        state = START;
        bytes = write(fd, DISC, 5);
        printf("sent DISC -> %d bytes written\n", bytes);
        // Wait until all bytes have been written to the serial port
        sleep(1);

        alarm(3);
        alarmEnabled = TRUE;

        while (state != STOP_S)
        {
            bytes = read(fd, buf, 1);
            if (bytes > 0)
            {
                switch (state)
                {
                case START:
                    if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    break;

                case FLAG_RCV:
                    if (buf[0] == A)
                    {
                        state = A_RCV;
                    }
                    else if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case A_RCV:
                    if (buf[0] == C_DISC)
                    {
                        state = C_RCV;
                    }
                    else if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case C_RCV:
                    if (buf[0] == (BCC_DISC))
                    {
                        state = BCC_OK;
                    }
                    else if (buf[0] == FLAG)
                    {
                        state = FLAG_RCV;
                    }
                    else
                    {
                        state = START;
                    }

                    break;

                case BCC_OK:
                    if (buf[0] == FLAG)
                    {
                        state = STOP_S;
                        DISC_RCV = TRUE;
                    }
                    else
                    {
                        state = START;
                    }
                    break;

                default:
                    break;
                }
            }
        }
    }

    if (DISC_RCV)
    {
        printf("success DISC received\n");
    }

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
    }*/

    bytes = write(fd, UA, 5);
    printf("sent UA -> %d bytes written\n", bytes);

    return 0;
}
