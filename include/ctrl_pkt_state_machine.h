#ifndef _CTRL_PKT_STATE_MACHINE_
#define _CTRL_PKT_STATE_MACHINE_

enum ControlState {C_START, C_CTRL_FIELD_RCV, C_TYPE_RCV, C_LENGTH_RCV, C_VALUE_RCV, C_STOP};

void control_start_transition_check(char byte_rcv);

void control_control_field_transition_check(char byte_rcv);

void control_type_transition_check(char byte_rcv);

void control_length_transition_check(char byte_rcv);

void control_value_transition_check(char byte_rcv);

int control_state_machine(int fd);

#endif // _CTRL_PKT_STATE_MACHINE_