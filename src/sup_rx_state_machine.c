#include <unistd.h>

#include "sup_rx_state_machine.h"
#include "link_layer.h"

enum State state;
unsigned char control_rcv;

void rx_start_transition_check(unsigned char byte_rcv) {
    if (byte_rcv == FLAG)
        state = FLAG_RCV;
}

void rx_flag_rcv_transition_check(unsigned char byte_rcv) {
    if (byte_rcv == ADDRESS)
        state = A_RCV;
    else if (byte_rcv != FLAG)
        state = START;
}

void rx_a_rcv_transition_check(unsigned char byte_rcv) {
    if (byte_rcv == SET_CONTROL || byte_rcv == UA_CONTROL || byte_rcv == DISC_CONTROL) {
        state = C_RCV;
        control_rcv = byte_rcv;
    }
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void rx_c_rcv_transition_check(unsigned char byte_rcv) {
    if (byte_rcv == (ADDRESS ^ control_rcv))
        state = BCC_OK;
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void rx_bcc_ok_transition_check(unsigned char byte_rcv) {
    if (byte_rcv == FLAG)
        state = STOP;
    else
        state = START;
}

int rx_state_machine(int fd) {
    unsigned char byte_rcv[BYTE_SIZE];
    state = START;
    while (state != STOP) {
        read(fd, byte_rcv, BYTE_SIZE);

        switch (state) {
        case START:
            rx_start_transition_check(byte_rcv[0]); break;
        case FLAG_RCV:
            rx_flag_rcv_transition_check(byte_rcv[0]); break;
        case A_RCV:
            rx_a_rcv_transition_check(byte_rcv[0]); break;
        case C_RCV:
            rx_c_rcv_transition_check(byte_rcv[0]); break;
        case BCC_OK:
            rx_bcc_ok_transition_check(byte_rcv[0]); break;
        default:
            break;
        }
    }
    return 0;
}
