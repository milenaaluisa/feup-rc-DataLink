#include <fcntl.h>
#include <signal.h>

#include "data-link.h"
#include "utils.h"
#include "transmitter-state-machine.h"

int alarm_enabled = 0;
int alarm_count = 0;
extern char control_rcv[BYTE_SIZE];

void alarm_handler(int signal) {
    alarm_enabled = 0;
    alarm_count++;
}

int start_transmission(int fd) {
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

    char* set_frame = assemble_supervision_frame(SET_CONTROL);

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            write(fd, set_frame, SUP_FRAME_SIZE);
            printf("Supervision frame sent\n");
            alarm(3);
            alarm_enabled = 1;
        }
        if (!state_machine(fd)) 
            return 0;
    }
    printf("Transmission failed\n");
    return 1;
}

/* commented for now because of changes to function assemble_information_frame()
int data_transfer(int fd, char* data, int num_packets) {
    int ns = 0, nr;
    int num_successful_packets = 0;
    int reply_from_receiver;
    char control_field = assemble_info_frame_ctrl_field (ns);
    char* info_frame = assemble_information_frame(control_field, data);

    while (num_successful_packets < num_packets) {
        (void) signal(SIGALRM, alarm_handler);
        reply_from_receiver = 0;

        while (alarm_count < 3) {
            if (!alarm_enabled) {
                write(fd, info_frame, INFO_FRAME_SIZE);
                printf("Information frame sent\n");
                alarm(3);
                alarm_enabled = 1;
            }

            if (!state_machine(fd)) { 
                nr = control_rcv[0] & BIT(7);
                if (control_rcv[0] & RR_ACK == RR_ACK && nr != ns) {
                    num_successful_packets++;
                    data += DATA_FIELD_BYTES;
                    ns = (ns == 0) ? 1 : 0;
                    control_field = assemble_info_frame_ctrl_field(ns);
                    info_frame =  assemble_information_frame(control_field, data);
                }
                reply_from_receiver = 1;
                break;
            }
        }

        if (!reply_from_receiver) {
            printf("Transmission failed\n");
            return 1;
        }
    }
    return 0;
} */

int stop_transmission(int fd) {
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

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
        if (!state_machine(fd) && control_rcv[0] == DISC_CONTROL) {
            printf("Disconnection frame read\n");

            write(fd, ua_frame, SUP_FRAME_SIZE);
            printf("Acknowledgement frame sent\n");
            return 0;
        }
    }
    printf("Disconnection failed\n");
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

    if (start_transmission(fd)) 
        return 1;

    /* Testing information frame assembling
    char control_field = BIT(6);
    char* packet = (char*) malloc(DATA_FIELD_BYTES);
    packet = "Helloooooooo world}~";
    char* frame = (char*) malloc(INFO_FRAME_SIZE);
    int frame_size = assemble_information_frame(control_field, packet, frame);
    for (int i = 0; i < frame_size; i++) {
        printf("%08x, %c\n", frame[i], frame[i]);
    }*/

    /*
    if (stop_transmission(fd))
        return 1;
    */
    return 0;
}
