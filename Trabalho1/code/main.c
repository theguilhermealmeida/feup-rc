// Main file of the serial port project.
// NOTE: This file must not be changed.

#include <stdio.h>
#include <stdlib.h>

#include "application_layer.h"
#include "utils.h"

#define N_TRIES 3
#define TIMEOUT 4
#define BAUD_RATE B38400

// Arguments:
//   $1: /dev/ttySxx
//   $2: tx | rx
//   $3: filename
  
void menu()
{
    printf("#################\n");
    printf("       MENU      \n");
    printf("#################\n\n");


    char* port;
    int role;
    int ntries;
    int timeout;
    char* filename;

    printf("Serial port (/dev/ttySxx): ");
    port = getInputString();


    printf("\nWhat's your role? ('0' for receiver, '1' for trasmitter): ");
    role = getInputInt(0, 1);

    printf("\nNumber of tries before aborting connection: ");
    ntries = getInputInt(1, 15);

    printf("\nTime in seconds to wait until a timeout: ");
    timeout = getInputInt(1, 15);

    if (role == 0) {
        printf("\nName for the received file: ");
	filename = getInputString();

        applicationLayer(port, "rx", BAUD_RATE, ntries, timeout, filename);
    }

    else {
        printf("\nFile to send:  ");
	filename = getInputString();

        applicationLayer(port, "tx", BAUD_RATE, ntries, timeout, filename);
    }

}

int main(int argc, char *argv[])
{
    if (argc != 1 && argc != 4) {
        printf("Usage: %s /dev/ttySxx tx|rx filename\n\n", argv[0]);
        printf("Usage for menu: %s \n", argv[0]);
	exit(1);
    }

    if (argc == 1) {
	menu();
    }
    else if (argc == 4) {
        const char *serialPort = argv[1];
	const char *role = argv[2];
	const char *filename = argv[3];
        applicationLayer(serialPort, role, BAUD_RATE, N_TRIES, TIMEOUT, filename);
    }
		 

	/*
    if (argc < 4)
    {
        printf("Usage: %s /dev/ttySxx tx|rx filename\n", argv[0]);
        exit(1);
    }

    const char *serialPort = argv[1];
    const char *role = argv[2];
    const char *filename = argv[3];

    printf("Starting link-layer protocol application\n"
           "  - Serial port: %s\n"
           "  - Role: %s\n"
           "  - Baudrate: %d\n"
           "  - Number of tries: %d\n"
           "  - Timeout: %d\n"
           "  - Filename: %s\n",
           serialPort,
           role,
           BAUD_RATE,
           N_TRIES,
           TIMEOUT,
           filename);

    applicationLayer(serialPort, role, BAUD_RATE, N_TRIES, TIMEOUT, filename);
    */

    return 0;
}

