#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "receiver.h"
#include "link_layer.h"
#include "utils.h"
#include "sup_rx_state_machine.h"
#include "info_state_machine.h"

int rx_start_transmission(int fd) {
    rx_state_machine(fd);
    printf("SET supervision frame read\n");

    char* ua_frame = assemble_supervision_frame(UA_CONTROL);
    write(fd, ua_frame, SUP_FRAME_SIZE);
    printf("UA supervision frame sent\n");
    return 0;
}

int rx_stop_transmission(int fd) {
    rx_state_machine(fd);
    printf("DISC supervision frame read\n");

    char* disc_frame = assemble_supervision_frame(DISC_CONTROL);
    write(fd, disc_frame, SUP_FRAME_SIZE);
    printf("DISC supervision frame sent\n");

    rx_state_machine(fd);
    printf("UA supervision frame read\n");
    return 0;
}

// TODO: Test
int receive_data(int fd, char* data, int num_packets) {
    int ns = 0;
    int num_successful_packets = 0;
    char* data_rcv = (char*) malloc(DATA_FIELD_BYTES);
    char* acknowledgement = (char*) malloc(SUP_FRAME_SIZE);
    char control_field;

    while (num_successful_packets < num_packets) {
        int has_error = info_frame_state_machine(fd, ns, data_rcv);
        if (!has_error) {
            ns = (ns == 0) ? 1 : 0;
            control_field = assemble_rr_frame_ctrl_field(ns);
            acknowledgement = assemble_supervision_frame(control_field);
            memcpy(data + num_successful_packets*DATA_FIELD_BYTES, data_rcv, DATA_FIELD_BYTES);
            num_successful_packets++;
        }
        else if (has_error == 2) {
            control_field = assemble_rr_frame_ctrl_field(ns);
            acknowledgement = assemble_supervision_frame(control_field);
        }
        else if (has_error == 3) {
            control_field = assemble_rej_frame_ctrl_field(ns);
            acknowledgement = assemble_supervision_frame(control_field);
        }
        write(fd, acknowledgement, SUP_FRAME_SIZE);
    }
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

    Draft testing receive_data
    char* data = (char*) malloc(DATA_FIELD_BYTES * 4);
    receive_data(fd, data, 4);

    if (stop_transmission(fd)){ 
            printf("Disconnection failed. \n");
            return 1;
    }
    
    return 0;
}*/
