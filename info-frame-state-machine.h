#include <unistd.h>

#include "data-link.h"

enum InfoState {I_START, I_FLAG_RCV, I_A_RCV, I_C_RCV, BCC1_RCV, DATA_RCV, BCC2_RCV, I_STOP};
enum InfoState info_state = I_START;

char control_rcv;
int has_error, byte_seq, is_escaped;

char data_rcv[DATA_FIELD_BYTES];

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

void info_a_rcv_transition_check(char byte_rcv, int ns) {
    char expected_rcv = assemble_info_frame_ctrl_field(ns);
    if (byte_rcv == expected_rcv) {
        info_state = I_C_RCV;
        control_rcv = byte_rcv;
    }
    else {
        info_state = I_START;
        has_error = 2;
    }
}

void info_c_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == (ADDRESS ^ control_rcv))
        info_state = BCC1_RCV;
    else {
        info_state = I_START;
        has_error = (has_error == 2) ? 2 : 1;
    }
}

void info_bcc1_rcv_transition_check(char byte_rcv) {
    if (byte_seq == DATA_FIELD_BYTES) {
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
    if (byte_rcv == generate_bcc2(data_rcv))
        info_state = BCC2_RCV;
    else {
        info_state = I_START;
        has_error = (has_error == 2) ? 2 : 3;
    }
}

void info_bcc2_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        info_state = I_STOP;
    else
        has_error = (has_error == 0) ? 1 : has_error;
}

int info_frame_state_machine(int fd, int ns, char* data_rcv) {
    memset(data_rcv, 0, DATA_FIELD_BYTES);
    // has_error = 1 indicates an error in the frame's header
    // has_error = 2 indicates that the frame being received is the worng one (duplicated)
    // has_error = 3 indicates an error in the frame's data field
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
            info_a_rcv_transition_check(byte_rcv[0], ns); break;
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