#ifndef _SUP_RX_STATE_MACHINE_
#define _SUP_RX_STATE_MACHINE_

enum State {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};

void rx_start_transition_check(char byte_rcv);

void rx_flag_rcv_transition_check(char byte_rcv);

void rx_a_rcv_transition_check(char byte_rcv);

void rx_c_rcv_transition_check(char byte_rcv);

void rx_bcc_ok_transition_check(char byte_rcv);

int rx_state_machine(int fd);

#endif