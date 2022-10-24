#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "transmitter.h"
#include "link_layer.h"
#include "utils.h"
#include "sup_tx_state_machine.h"

int alarm_enabled, alarm_count;
extern char control_rcv;
int ns, nr;

void alarm_handler(int signal) {
    alarm_enabled = 0;
    alarm_count++;
}

int tx_start_transmission(int fd) {
    alarm_enabled = 0;
    alarm_count = 0;
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

    char* set_frame = assemble_supervision_frame(SET_CONTROL);

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            write(fd, set_frame, SUP_FRAME_SIZE);
            printf("SET supervision frame sent\n");
            alarm(3);
            alarm_enabled = 1;
        }
        if (!tx_state_machine(fd)) {
            ns = 0;
            printf("UA supervision frame read\n");
            return 0;
        }
    }
    printf("Transmission failed\n");
    return 1;
}

int tx_stop_transmission(int fd) {
    alarm_enabled = 0;
    alarm_count = 0;
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

    char* disc_frame = assemble_supervision_frame(DISC_CONTROL);
    char* ua_frame = assemble_supervision_frame(UA_CONTROL);

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            write(fd, disc_frame, SUP_FRAME_SIZE);
            alarm(3);
            alarm_enabled = 1;
            printf("DISC supervision frame sent\n");
        }
        if (!tx_state_machine(fd) && control_rcv == DISC_CONTROL) {
            printf("DISC supervision frame read\n");

            write(fd, ua_frame, SUP_FRAME_SIZE);
            printf("UA supervision frame sent\n");
            return 0;
        }
    }
    printf("Disconnection failed\n");
    return 1;
}

int send_info_frame(int fd, char* buffer, int buffer_size) {
    char control_field = assemble_info_frame_ctrl_field(ns);
    int info_frame_size;
    char* info_frame = assemble_information_frame(control_field, buffer, buffer_size, &info_frame_size);

    alarm_enabled = 0;
    alarm_count = 0;
    (void) signal(SIGALRM, alarm_handler);
    while (alarm_count < 3) { 
        if (!alarm_enabled || resend) {
            resend = 0;
            write(fd, info_frame, info_frame_size);
            printf("Information frame sent\n");
            alarm(3);
            alarm_enabled = 1;
        }

        if (!tx_state_machine(fd)) { 
            nr = control_rcv & BIT(7);
            if ((control_rcv & RR_ACK) == RR_ACK && nr != ns)
                break;
            else if ((control_rcv & RR_ACK) == RR_ACK && nr == ns) {
                resend = 1;
            }
            else if ((control_rcv & REJ_ACK) == REJ_ACK) {
                alarm(3);
                alarm_enabled = 0;
            }
        }
    }

    ns = (ns == 0) ? 1 : 0;
    return 0;
}

/*
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

    if (start_transmission(fd)) 
        return 1;

    Testing information frame assembling
    char control_field = BIT(6);
    char* packet = (char*) malloc(DATA_FIELD_BYTES);
    packet = "Helloooooooo world}~";
    char* frame = (char*) malloc(INFO_FRAME_SIZE);
    int frame_size = assemble_information_frame(control_field, packet, frame);
    for (int i = 0; i < frame_size; i++) {
        printf("%08x, %c\n", frame[i], frame[i]);
    }

    
    if (stop_transmission(fd))
        return 1;

    return 0;
}*/
