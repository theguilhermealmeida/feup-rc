// Link layer protocol implementation

#include "link_layer.h"

#define NO_DATA nodata
#define NO_COUNT 0

volatile int STOP = FALSE;

extern int finish;

STATE state = START;

int alarmEnabled = FALSE;
int alarmCount = 0;
int ns = 0;
int nr = 1;
int RRsent = 0;
int REJsent = 0;
int RRreceived = 0;
int REJreceived = 0;

unsigned char nodata[0];

struct termios oldtio;
struct termios newtio;

int baudRate;
int nTries;
int timeout;

// Alarm function handler
void alarmHandler(int signal)
{
    alarmEnabled = FALSE;
    alarmCount++;
    state = STOP_ALARM;

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

    timeout = connectionParameters.timeout;
    baudRate = connectionParameters.baudRate;
    nTries = connectionParameters.nRetransmissions;

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = baudRate | CS8 | CLOCAL | CREAD;
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


    if (connectionParameters.role) // if recetor
    {
        unsigned char buf[1] = {0};

        int bytes;
        while (state != STOP_ALARM && state != STOP_SET)
        {
            bytes = read(fd, buf, 1);
            // printf("%d\n", buf[0]);
            if (bytes > 0)
            {
                state = state_machine(buf[0], state, NO_DATA, NO_COUNT);
            }
        }

        //printf("Success SET received\n");
        sendFrame(fd, C_UA, BCC_UA);
        //printf("Sent UA -> %d bytes written\n", bytes);
    }

    if (!connectionParameters.role) // if emissor
    {
        unsigned char buf[1] = {0};

        while (!UA_RCV)
        {
            if (alarmCount >= nTries)
            {
                printf("Maximum tries reached.Ending program\n");
                return -1;
            }
            state = START;

            sendFrame(fd, C_SET, BCC_SET);
            //printf("Sent SET -> %d bytes written\n", bytes);

            // Wait until all bytes have been written to the serial port
            sleep(1);

            alarm(timeout);
            alarmEnabled = TRUE;

            while (state != STOP_ALARM && state != STOP_UA)
            {
                int bytes = read(fd, buf, 1);
                if (bytes > 0)
                {
                    state = state_machine(buf[0], state, NO_DATA, NO_COUNT);
                }
            }

            if (state == STOP_UA)
            {
                UA_RCV = TRUE;
                //printf("Success UA received\n");
            }
        }
    }
    return fd;
}

////////////////////////////////////////////////
// LLWRITE
////////////////////////////////////////////////
int llwrite(int fd, const unsigned char *buf, int bufSize)
{
    unsigned char buffer[1] = {0}; // +1: Save space for the final '\0' char
    int RR_RCV = FALSE;
    alarmCount = 0;
    unsigned char newBuff[PACKET_SIZE * 2] = {0};

    createBCC(buf, newBuff, bufSize);
    int size = byte_stuffing(newBuff, bufSize + 1);


    while (!RR_RCV)
    {
        if (alarmCount >= nTries)
        {
            printf("Maximum tries reached. Ending program\n");
            return -1;
        }
        state = START;
        //printf("size: %d\n",size);
        sendInformationFrame(fd, ns << 6, A ^ (ns << 6), newBuff, size);
        //printf("Sent I -> %d bytes written\n", bytes);

        sleep(1);

        alarm(timeout);
        alarmEnabled = TRUE;

        while (state != STOP_ALARM && state != STOP_RR && state != STOP_REJ)
        {
            int bytes = read(fd, buffer, 1);
            if (bytes > 0)
            {
                state = state_machine(buffer[0], state, NO_DATA, NO_COUNT);
            }
        }

        if (state == STOP_RR)
        {
            RR_RCV = TRUE;
            ns ^= 1;
            RRreceived++;
            //printf("Success RR received\n");
        }
        else if (state == STOP_REJ)
        {
            REJreceived++;
            //printf("Success REJ received\n");
        }
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
    unsigned char buf[1] = {0};
    unsigned char data[PACKET_SIZE * 2] = {0};
    int count_data = 0;
    int bytes;

    while (state != STOP_ALARM && state != STOP_SET && state != STOP_DISC && state != STOP_DATA && state != STOP_DATA_RPT)
    {
        bytes = read(fd, buf, 1);
        if (bytes > 0)
        {
            state = state_machine(buf[0], state, data, &count_data);
        }
    }

    int size = byte_destuffing(data, count_data);

    if (state == STOP_SET)
    {
        //printf("Success SET received\n");
        sendFrame(fd, C_UA, BCC_UA);
        //printf("Sent UA -> %d bytes written\n", bytes);
    }
    else if (state == STOP_DISC)
    {
        //printf("Success DISC received\n");

        while (!UA_RCV)
        {
            if (alarmCount >= nTries)
            {
                printf("Maximum tries reached. Ending program\n");
                finish = TRUE;
                return -1;
            }
            state = START;
            sendFrame(fd, C_DISC, BCC_DISC);
            //printf("Sent DISC -> %d bytes written\n", bytes);
            sleep(1);

            alarm(timeout);
            alarmEnabled = TRUE;

            while (state != STOP_ALARM && state != STOP_UA)
            {
                bytes = read(fd, buf, 1);
                if (bytes > 0)
                {
                    state = state_machine(buf[0], state, NO_DATA, NO_COUNT);
                }
            }

            if (state == STOP_UA)
            {
                UA_RCV = TRUE;
                //printf("Success UA received\n");
                finish = TRUE;

                if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
                {
                    perror("tcsetattr");
                    exit(-1);
                }

                close(fd);
                printStatistics(0,RRsent,REJsent);
                return 0;
            }
        }
    }
    else if (state == STOP_DATA)
    {
        //printf("Success DATA received\n");
        unsigned char BCC2 = 0;

        for (int i = 0; i < size - 1; i++)
        {
            BCC2 = BCC2 ^ data[i];
        }

        if (BCC2 == data[size - 1])
        {
            //printf("BCC2 ok!\n");
            for (int i = 0; i < size - 1; i++)
            {
                packet[i] = data[i];
            }
            sendFrame(fd, C_RR ^ (nr << 7), A ^ (C_RR ^ (nr << 7)));
            nr ^= 1;
            RRsent++;
            //printf("sent RR -> %d bytes written\n", bytes);
            sleep(1);
        }
        else
        {
            //printf("BCC2 FAILED!\n");
            sendFrame(fd, C_REJ ^ (nr << 7), A ^ (C_REJ ^ (nr << 7)));
            REJsent++;
            //printf("sent REJ -> %d bytes written\n", bytes);
            sleep(1);
        }
    }
    else if (state == STOP_DATA_RPT)
    {
        //printf("Success DATA_RPT received\n");
        unsigned char BCC2 = 0;

        for (int i = 0; i < size - 1; i++)
        {
            BCC2 = BCC2 ^ data[i];
        }

        if (BCC2 == data[size - 1])
        {
            //printf("BCC2 ok!\n");
            sendFrame(fd, C_RR ^ ((nr ^ 1) << 7), A ^ (C_RR ^ ((nr ^ 1) << 7)));
            //printf("sent RR -> %d bytes written\n", bytes);
            RRsent++;
            sleep(1);
        }
        else
        {
            //printf("BCC2 FAILED!\n");
            sendFrame(fd, C_RR ^ ((nr ^ 1) << 7), A ^ (C_RR ^ ((nr ^ 1) << 7)));
            //printf("sent RR -> %d bytes written\n", bytes);
            RRsent++;
            sleep(1);
        }
    }

    return 0;
}

////////////////////////////////////////////////
// LLCLOSE
////////////////////////////////////////////////
int llclose(int fd)
{
    unsigned char buf[1] = {0};
    alarmCount = 0;
    int DISC_RCV = FALSE;
    int bytes;
    alarmCount = 0;

    while (!DISC_RCV)
    {
        if (alarmCount >= nTries)
        {
            printf("Maximum tries reached. Ending program\n");
            return -1;
        }
        state = START;

        sendFrame(fd, C_DISC, BCC_DISC);
        //printf("Sent DISC -> %d bytes written\n", bytes);

        // Wait until all bytes have been written to the serial port
        sleep(1);

        alarm(timeout);
        alarmEnabled = TRUE;

        while (state != STOP_ALARM && state != STOP_DISC)
        {
            bytes = read(fd, buf, 1);
            if (bytes > 0)
            {
                state = state_machine(buf[0], state, NO_DATA, NO_COUNT);
            }
        }

        if (state == STOP_DISC)
        {
            //printf("success DISC received\n");
            DISC_RCV = TRUE;
            sendFrame(fd, C_UA, BCC_UA);
            //printf("Sent UA -> %d bytes written\n", bytes);
            sleep(1);
        }
    }

    printStatistics(1,RRreceived,REJreceived);

    return 0;
}
