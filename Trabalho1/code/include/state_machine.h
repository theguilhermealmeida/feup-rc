#ifndef _STATE_MACHINE_H_
#define _STATE_MACHINE_H_

#include "utils.h"

typedef enum
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    C_SET_RCV,
    C_RR_RCV,
    C_UA_RCV,
    C_REJ_RCV,
    C_DISC_RCV,
    C_DATA_RCV,
    C_DATA_RPT_RCV,
    BCC_OK,
    BCC_SET_OK,
    BCC_UA_OK,
    BCC_DISC_OK,
    BCC_DATA_OK,
    BCC_DATA_RPT_OK,
    BCC_RR_OK,
    BCC_REJ_OK,
    DATA_RCV,
    DATA_RPT_RCV,
    STOP_DISC,
    STOP_S,
    STOP_ALARM,
    STOP_SET,
    STOP_UA,
    STOP_RR,
    STOP_REJ,
    STOP_DATA,
    STOP_DATA_RPT
} STATE;

STATE state_machine(unsigned char buf ,STATE state, unsigned char * data,int* count_data);

#endif // _STATE_MACHINE_H_