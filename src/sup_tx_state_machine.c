#include <unistd.h>

#include "sup_tx_state_machine.h"
#include "link_layer.h"

enum State state;
char control_rcv;

void start_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = FLAG_RCV;
}

void flag_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == ADDRESS)
        state = A_RCV;
    else if (byte_rcv != FLAG)
        state = START;
}

void a_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == UA_CONTROL || byte_rcv == DISC_CONTROL ||
        (byte_rcv & RR_ACK) == RR_ACK || 
        (byte_rcv & REJ_ACK) == REJ_ACK) {
        state = C_RCV;
        control_rcv = byte_rcv;
    }
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void c_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == (ADDRESS ^ control_rcv))
        state = BCC_OK;
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void bcc_ok_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = STOP;
    else
        state = START;
}

int state_machine(int fd) {
    char byte_rcv[BYTE_SIZE];
    state = START;
    if (!read(fd, byte_rcv, BYTE_SIZE)) 
        return 1;

    while (state != STOP) {
        if (state != START)
            read(fd, byte_rcv, BYTE_SIZE);
        // printf("%08x\n", byte_rcv[0]);

        switch (state) {
        case START:
            start_transition_check(byte_rcv[0]); break;
        case FLAG_RCV:
            flag_rcv_transition_check(byte_rcv[0]); break;
        case A_RCV:
            a_rcv_transition_check(byte_rcv[0]); break;
        case C_RCV:
            c_rcv_transition_check(byte_rcv[0]); break;
        case BCC_OK:
            bcc_ok_transition_check(byte_rcv[0]); break;
        default:
            break;
        }
    }

    // printf("Supervision frame received back from receiver\n");
    return 0;
}
