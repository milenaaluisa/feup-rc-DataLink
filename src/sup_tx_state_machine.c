#include <unistd.h>
#include <stdio.h>

#include "sup_tx_state_machine.h"
#include "link_layer.h"

enum State state;
char control_rcv;

void tx_start_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = FLAG_RCV;
}

void tx_flag_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == ADDRESS)
        state = A_RCV;
    else if (byte_rcv != FLAG)
        state = START;
}

void tx_a_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == UA_CONTROL || byte_rcv == DISC_CONTROL ||
        byte_rcv == RR_ACK ||  byte_rcv == (char) (RR_ACK | SET_SUP_FRAME_CONTROL) ||
        byte_rcv == REJ_ACK || byte_rcv == (char) (REJ_ACK | SET_SUP_FRAME_CONTROL)) {
        state = C_RCV;
        control_rcv = byte_rcv;
    }
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void tx_c_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == (ADDRESS ^ control_rcv))
        state = BCC_OK;
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void tx_bcc_ok_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = STOP;
    else
        state = START;
}

int tx_state_machine(int fd) {
    char byte_rcv[BYTE_SIZE];
    state = START;
    if (!read(fd, byte_rcv, BYTE_SIZE)) 
        return 1;

    while (state != STOP) {
        if (state != START)
            read(fd, byte_rcv, BYTE_SIZE);

        switch (state) {
        case START:
            tx_start_transition_check(byte_rcv[0]); break;
        case FLAG_RCV:
            tx_flag_rcv_transition_check(byte_rcv[0]); break;
        case A_RCV:
            tx_a_rcv_transition_check(byte_rcv[0]); break;
        case C_RCV:
            tx_c_rcv_transition_check(byte_rcv[0]); break;
        case BCC_OK:
            tx_bcc_ok_transition_check(byte_rcv[0]); break;
        default:
            break;
        }
    }
    return 0;
}
