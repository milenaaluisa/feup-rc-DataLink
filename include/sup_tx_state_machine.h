#ifndef _SUP_TX_STATE_MACHINE_
#define _SUP_TX_STATE_MACHINE_

enum State {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};

void tx_start_transition_check(unsigned char byte_rcv);

void tx_flag_rcv_transition_check(unsigned char byte_rcv);

void tx_a_rcv_transition_check(unsigned char byte_rcv);

void tx_c_rcv_transition_check(unsigned char byte_rcv);

void tx_bcc_ok_transition_check(unsigned char byte_rcv);

int tx_state_machine(int fd);

#endif // _SUP_TX_STATE_MACHINE_
