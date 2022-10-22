#ifndef _INFO_STATE_MACHINE_
#define _INFO_STATE_MACHINE_

enum InfoState {I_START, I_FLAG_RCV, I_A_RCV, I_C_RCV, BCC1_RCV, DATA_RCV, BCC2_RCV, I_STOP};

void info_start_transition_check(char byte_rcv);

void info_flag_rcv_transition_check(char byte_rcv);

void info_a_rcv_transition_check(char byte_rcv, int ns);

void info_c_rcv_transition_check(char byte_rcv);

void info_bcc1_rcv_transition_check(char byte_rcv);

void info_data_rcv_transition_check(char byte_rcv);

void info_bcc2_rcv_transition_check(char byte_rcv);

int info_frame_state_machine(int fd, int ns, char* data_rcv);

#endif // _INFO_STATE_MACHINE_