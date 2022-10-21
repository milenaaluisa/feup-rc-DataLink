#include <fcntl.h>

#include "data-link.h"
#include "utils.h"
#include "receiver-state-machine.h"
#include "info-frame-state-machine.h"

int send_back_disc_frame(int fd) {
    char* disc_frame = assemble_supervision_frame(DISC_CONTROL);
    char* ua_frame_rcv = malloc(SUP_FRAME_SIZE);

    write(fd, disc_frame, SUP_FRAME_SIZE);
    printf("Disconnection frame sent\n");

    if (read(fd, ua_frame_rcv, SUP_FRAME_SIZE)) {
        for (int i = 0; i < SUP_FRAME_SIZE; i++)
            printf("%08x\n", ua_frame_rcv[i]);
        printf("Acknowledgement frame read\n");
        return 0;
    }
    return 1;
}

// TODO: Test
int receive_data(int fd, char* data, int num_packets) {
    int ns = 0, nr;
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
    }
    return 0; 
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

    state_machine(fd);
    printf("Supervision frame read\n");

    char* ua_frame = assemble_supervision_frame(UA_CONTROL);
    write(fd, ua_frame, SUP_FRAME_SIZE);
    printf("Acknowledgement frame sent\n");

    /* Draft testing receive_data
    char* data = (char*) malloc(DATA_FIELD_BYTES * 4);
    receive_data(fd, data, 4); */

    // TODO: Fix disconnection
    /*
    if (control_rcv[0] == DISC_CONTROL){
        if (send_back_disc_frame(fd) != 0) 
            printf("Disconnection failed. \n");
            return 1;
    }
    */

    return 0;
}
