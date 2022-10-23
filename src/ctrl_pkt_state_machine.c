#include <unistd.h>
#include <string.h>


#include "link_layer.h"
#include "utils.h"
#include "ctrl_pkt_state_machine.h"

enum ControlState control_state;


void control_start_transition_check(char byte_rcv){
    if (byte_rcv == C_START)
        control_state = C_CTRL_FIELD_RCV;
}

void control_control_field_transition_check(char byte_rcv){
    if (byte_rcv == C_CTRL_FIELD_RCV)
        control_state = C_TYPE_RCV;
}

// como verifico se é TYPE_FILE_SIZE ou TYPE_FILE_NAME
void control_type_transition_check(char byte_rcv){
    if (byte_rcv == C_TYPE_RCV)
        control_state = C_LENGTH_RCV;
    else 
        control_state = C_START;
}

void control_length_transition_check(char byte_rcv){
    if (byte_rcv == C_LENGTH_RCV)
        control_state = C_VALUE_RCV;
    else 
        control_state = C_START;
}

void control_value_transition_check(char byte_rcv){
    if (byte_rcv == C_VALUE_RCV)
        control_state = C_STOP;
    else 
        control_state = C_START;
}

int control_state_machine(int fd){

    control_state = C_START;
    char byte_rcv[BYTE_SIZE];

    while (control_state != C_STOP){
        read (fd, byte_rcv, BYTE_SIZE);

        switch (control_state){
            case C_START:
                control_start_transition_check(byte_rcv[0]); break;
            case C_CTRL_FIELD_RCV:
                control_control_field_transition_check(byte_rcv[0]); break;
            case C_TYPE_RCV:
                control_type_transition_check(byte_rcv[0]); break;
            case C_LENGTH_RCV:
                control_length_transition_check(byte_rcv[0]); break;
            case C_VALUE_RCV:
                control_value_transition_check(byte_rcv[0]); break;
            default:
                break;
        }
    }

    return 0;

}