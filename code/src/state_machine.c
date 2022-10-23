#include "state_machine.h"

extern int ns;
extern int nr;

STATE state_machine(unsigned char buf, STATE state, unsigned char *data, int *count_data)
{
    // printf("ns: %d\n", ns);
    // printf("nr: %d\n", nr);
    // printf("state: %d\n", (int)state);
    // printf("%d\n", buf);

    switch (state)
    {
    case START:
        if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        break;

    case FLAG_RCV:
        if (buf == A)
        {
            state = A_RCV;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case A_RCV:
        if (buf == C_SET)
        {
            state = C_SET_RCV;
        }
        else if (buf == C_UA)
        {
            state = C_UA_RCV;
        }
        else if (buf == (C_RR ^ ((ns ^ 1) << 7)))
        {
            state = C_RR_RCV;
        }
        else if (buf == (C_REJ ^ ((ns ^ 1) << 7)))
        {
            state = C_REJ_RCV;
        }
        else if (buf == ((nr ^ 1) << 6))
        {
            state = C_DATA_RCV;
        }
        else if (buf == (nr << 6))
        {
            state = C_DATA_RPT_RCV;
        }
        else if (buf == C_DISC)
        {
            state = C_DISC_RCV;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }
        break;

    case C_SET_RCV:
        if (buf == (BCC_SET))
        {
            state = BCC_SET_OK;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case C_UA_RCV:
        if (buf == (BCC_UA))
        {
            state = BCC_UA_OK;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case C_RR_RCV:
        if (buf == (C_RR ^ ((ns ^ 1) << 7) ^ A))
        {
            state = BCC_RR_OK;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case C_REJ_RCV:
        if (buf == (C_REJ ^ ((ns ^ 1) << 7) ^ A))
        {
            state = BCC_REJ_OK;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case C_DATA_RCV:
        if (buf == (A ^ ((nr ^ 1) << 6)))
        {
            state = BCC_DATA_OK;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case C_DATA_RPT_RCV:
        if (buf == (A ^ (nr << 6)))
        {
            state = BCC_DATA_RPT_OK;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case C_DISC_RCV:
        if (buf == (BCC_DISC))
        {
            state = BCC_DISC_OK;
        }
        else if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            state = START;
        }

        break;

    case BCC_SET_OK:
        if (buf == FLAG)
        {
            state = STOP_SET;
        }
        else
        {
            state = START;
        }
        break;

    case BCC_UA_OK:
        if (buf == FLAG)
        {
            state = STOP_UA;
        }
        else
        {
            state = START;
        }
        break;

    case BCC_RR_OK:
        if (buf == FLAG)
        {
            state = STOP_RR;
        }
        else
        {
            state = START;
        }
        break;

    case BCC_REJ_OK:
        if (buf == FLAG)
        {
            state = STOP_REJ;
        }
        else
        {
            state = START;
        }
        break;

    case BCC_DATA_OK:
        if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            data[*count_data] = buf;
            (*count_data)++;
            state = DATA_RCV;
        }

        break;

    case BCC_DATA_RPT_OK:
        if (buf == FLAG)
        {
            state = FLAG_RCV;
        }
        else
        {
            data[*count_data] = buf;
            (*count_data)++;
            state = DATA_RPT_RCV;
        }

        break;

    case BCC_DISC_OK:
        if (buf == FLAG)
        {
            state = STOP_DISC;
        }
        else
        {
            state = START;
        }

        break;

    case DATA_RCV:
        if (buf == FLAG)
        {
            state = STOP_DATA;
        }
        else
        {
            data[*count_data] = buf;
            (*count_data)++;
        }
        break;

    case DATA_RPT_RCV:
        if (buf == FLAG)
        {
            state = STOP_DATA_RPT;
        }
        else
        {
            data[*count_data] = buf;
            (*count_data)++;
        }
        break;

    default:
        break;
    }
    return state;
}