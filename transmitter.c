#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>

#include "data-link.h"
#include "utils.h"


enum State {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};
enum State state = START;


char transm_control_rcv[BYTE_SIZE];

int alarm_enabled = 0;
int alarm_count = 0;

void transm_start_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = FLAG_RCV;
}



void transm_flag_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == ADDRESS)
        state = A_RCV;
    else if (byte_rcv != FLAG)
        state = START;
}



void transm_a_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == UA_CONTROL || byte_rcv == DISC_CONTROL) {
        state = C_RCV;
        transm_control_rcv[0] = byte_rcv;
    }
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}



void transm_c_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == (ADDRESS ^ UA_CONTROL))
        state = BCC_OK;
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}


void transm_bcc_ok_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = STOP;
    else
        state = START;
}


int transm_receive_inf_frame(int fd) {
    int is_escaped = 0;
    char byte_rcv[BYTE_SIZE];

    for (int i = 0; i < DATA_FIELD_SIZE; i++) {
        read(fd, byte_rcv, BYTE_SIZE);
        if (is_escaped) {
            printf("%c", *byte_rcv ^ STF_XOR);
            is_escaped = 0;
        }
        else if (*byte_rcv == ESCAPE)
            is_escaped = 1;
        else
            printf("%c", *byte_rcv);
    }
    printf("\n");
    return 0;
}



int transm_state_machine(int fd) {
    char byte_rcv[BYTE_SIZE];
    if (!read(fd, byte_rcv, BYTE_SIZE)) return 1;

    while (state != STOP) {
        if (state != START)
            read(fd, byte_rcv, BYTE_SIZE);
        printf("%08x\n", byte_rcv[0]);
        switch (state) {
        case START:
            transm_start_transition_check(byte_rcv[0]); break;
        case FLAG_RCV:
            transm_flag_rcv_transition_check(byte_rcv[0]); break;
        case A_RCV:
            transm_a_rcv_transition_check(byte_rcv[0]); break;
        case C_RCV:
            transm_c_rcv_transition_check(byte_rcv[0]); break;
        case BCC_OK:
            transm_bcc_ok_transition_check(byte_rcv[0]); break;
        }
    }
    printf("Supervision frame received back from receiver\n");
    return 0;

}


void alarm_handler(int signal) {
    alarm_enabled = 0;
    alarm_count++;
}

char* stuffing(char* data) {
    int stuffed_data_size = 0;
    char* stuffed_data = (char*) malloc(DATA_FIELD_SIZE);
    char* stuffed_data_ptr = stuffed_data;

    while (stuffed_data_size < DATA_FIELD_SIZE) {
        if (*data == FLAG || *data == ESCAPE) {
            *stuffed_data_ptr = ESCAPE;
            stuffed_data_ptr++;
            stuffed_data_size++;
            *stuffed_data_ptr = *data ^ STF_XOR;
        }
        else
            *stuffed_data_ptr = *data;

        data++;
        stuffed_data_ptr++;
        stuffed_data_size++;
    }
    return stuffed_data;
}

int stop_transmission(int fd) {
    char* disc_frame = assemble_supervision_frame(DISC_CONTROL);
    char* ua_frame = assemble_supervision_frame(UA_CONTROL);
    char* disc_frame_rcv = malloc(SUP_FRAME_SIZE);

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            write(fd, disc_frame, SUP_FRAME_SIZE);
            printf("Disconnection frame sent\n");
            alarm(3);
            alarm_enabled = 1;
        }
        if (read(fd, disc_frame_rcv, SUP_FRAME_SIZE)) {
            for (int i = 0; i < SUP_FRAME_SIZE; i++)
                printf("%08x\n", disc_frame_rcv[i]);
            printf("Disconnection frame read\n");

            write(fd, ua_frame, SUP_FRAME_SIZE);
            printf("Acknowledgement frame sent\n");
            return 0;
        }
    }
    return 1;
}

int  while_not_stop_alarm(int fd, char *set_frame){
    write(fd, set_frame, SUP_FRAME_SIZE);
    printf("Supervision frame sent\n");
    alarm(3);
    alarm_enabled = 1;
    /* testing stuffing */
    char* data = (char*) malloc(DATA_FIELD_SIZE);
    char* stuffed_data = (char*) malloc(DATA_FIELD_SIZE);
    data = "Helloooooo world}~"; // } is 0x7d, ~ is 0x7e, becomes: }] and }^
    stuffed_data = stuffing(data);
    printf("%s\n", stuffed_data);
    write(fd, stuffed_data, DATA_FIELD_SIZE);
    printf("Information frame sent\n");

    return 0;

}




int alarm_helper(int fd) {
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

    char* set_frame = assemble_supervision_frame(SET_CONTROL);
    char ua_frame[SUP_FRAME_SIZE];

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            if (while_not_stop_alarm(fd, set_frame)) return 1;
        }
        if (!transm_state_machine(fd)) 
            return 0;
    }
    printf("Transmission failed\n");
    return 1;

}



int main(int argc, char *argv[]) {
    const char *serialPortName = argv[1];
    if (argc < 2) {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (create_termios_structure(fd, serialPortName)) 
        return 1;
    printf("New termios structure set\n");

    if (alarm_helper(fd)) 
        return 1;
    return 0;

}
