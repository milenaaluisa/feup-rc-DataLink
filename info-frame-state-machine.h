#include <unistd.h>

#include "data-link.h"

enum InfoState {I_START, I_FLAG_RCV, I_A_RCV, I_C_RCV, BCC1_RCV, 
                DATA_RCV, BCC2_RCV, I_STOP};
enum InfoState info_state = I_START;

int byte_seq, has_error, is_escaped;

char data_rcv[DATA_FIELD_SIZE];

void info_start_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        info_state = I_FLAG_RCV;
}

void info_flag_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == ADDRESS)
        info_state = I_A_RCV;
    else {
        info_state = I_START;
        has_error = 1;
    }
}

void info_a_rcv_transition_check(char byte_rcv, int frame_to_rcv) {
    char expected_rcv = INFO_FRAME_CONTROL;
    if (frame_to_rcv)
        expected_rcv = SET_INFO_FRAME_CONTROL;

    if (byte_rcv == expected_rcv)
        info_state = I_C_RCV;
    else {
        info_state = I_START;
        has_error = 1;
    }
}

void info_c_rcv_transition_check(char byte_rcv) {
    char expected_bcc = 0x00; // TODO: change to call to function that calculates BCC
    if (byte_rcv == expected_bcc)
        info_state = BCC1_RCV;
    else {
        info_state = I_START;
        has_error = 1;
    }
}

void info_bcc1_rcv_transition_check(char byte_rcv) {
    if (byte_seq == DATA_FIELD_SIZE) {
        info_state = DATA_RCV;
        return;
    }

    if (is_escaped) {
        data_rcv[byte_seq] = byte_rcv ^ STF_XOR;
        byte_seq++;
        is_escaped = 0;
    }
    else if (byte_rcv == ESCAPE)
        is_escaped = 1;
    else {
        data_rcv[byte_seq] = byte_rcv;
        byte_seq++;
    }
}

void info_data_rcv_transition_check(char byte_rcv) {
    char expected_bcc = 0x00; // TODO: change to call to function that calculates BCC
    if (byte_rcv == expected_bcc)
        info_state = BCC2_RCV;
    else {
        info_state = I_START;
        has_error = 1;
    }
}

void info_bcc2_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        info_state = I_STOP;
    else
        has_error = 1;
}

int info_state_machine(int fd, int frame_to_rcv, char* data_rcv) {
    memset(data_rcv, 0, DATA_FIELD_SIZE);
    has_error = 0;
    is_escaped = 0;

    char byte_rcv[BYTE_SIZE];
    while (info_state != I_STOP) {
        read(fd, byte_rcv, BYTE_SIZE);
        if (has_error)
            continue;

        switch (info_state) {
        case I_START: 
            info_start_transition_check(byte_rcv[0]); break;
        case I_FLAG_RCV:
            info_flag_rcv_transition_check(byte_rcv[0]); break;
        case I_A_RCV: 
            info_a_rcv_transition_check(byte_rcv[0], frame_to_rcv); break;
        case I_C_RCV:
            info_c_rcv_transition_check(byte_rcv[0]); break;
        case BCC1_RCV:
            info_bcc1_rcv_transition_check(byte_rcv[0]); break;
        case DATA_RCV:
            info_data_rcv_transition_check(byte_rcv[0]); break;
        case BCC2_RCV:
            info_bcc2_rcv_transition_check(byte_rcv[0]); break;
        }
    } 
    return has_error;
}