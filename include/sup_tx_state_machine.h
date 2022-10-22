#ifndef _SUP_TX_STATE_MACHINE_
#define _SUP_TX_STATE_MACHINE_

enum State {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};

void start_transition_check(char byte_rcv);

void flag_rcv_transition_check(char byte_rcv);

void a_rcv_transition_check(char byte_rcv);

void c_rcv_transition_check(char byte_rcv);

void bcc_ok_transition_check(char byte_rcv);

int state_machine(int fd);

#endif
