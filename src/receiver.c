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

int ns;

int rx_start_transmission(int fd) {
    rx_state_machine(fd);
    printf("SET supervision frame read\n");

    char* ua_frame = assemble_supervision_frame(UA_CONTROL);
    write(fd, ua_frame, SUP_FRAME_SIZE);
    printf("UA supervision frame sent\n");

    ns = 0;
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
    printf("Successfully disconnected\n");
    return 0;
}

// TODO: Test
int receive_info_frame(int fd, char* packet) {
    char* data_rcv = (char*) malloc(DATA_FIELD_BYTES);
    char* acknowledgement = (char*) malloc(SUP_FRAME_SIZE);
    char control_field;

    int has_error = info_frame_state_machine(fd, ns, data_rcv);
    if (!has_error)
        ns = (ns == 0) ? 1 : 0;
    if (!has_error || has_error == 2) {
        control_field = assemble_rr_frame_ctrl_field(ns);
        printf("RR supervision frame sent\n");
    }
    else if (has_error == 3) {
        control_field = assemble_rej_frame_ctrl_field(ns);
        printf("REJ supervision frame sent\n");
    }
    if (has_error != 1)
        acknowledgement = assemble_supervision_frame(control_field);

    write(fd, acknowledgement, SUP_FRAME_SIZE);
    return 0;
}
