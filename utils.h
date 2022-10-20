#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>

#include "data-link.h"

char generate_bcc2(const char* data_rcv){
    char bcc2 = data_rcv[0];
    for (int i = 1; i < DATA_FIELD_SIZE; i++)
        bcc2 ^=  data_rcv[i];
    return bcc2;
}

char* assemble_supervision_frame(char control_field) {
    char* sup_frame = malloc(SUP_FRAME_SIZE);
    sup_frame[FLAG1_IDX] = FLAG;
    sup_frame[ADDRESS_IDX] = ADDRESS;
    sup_frame[CONTROL_IDX] = control_field;
    sup_frame[BCC1_IDX] = ADDRESS ^ control_field;
    sup_frame[S_FLAG2_IDX] = FLAG;

    return sup_frame;
}

char* assemble_information_frame(char control_field, const char* packet) {
    char* info_frame = malloc(INFO_FRAME_SIZE);
    info_frame[FLAG1_IDX] = FLAG;
    info_frame[ADDRESS_IDX] = ADDRESS;
    info_frame[CONTROL_IDX] = control_field;
    info_frame[BCC1_IDX] = ADDRESS ^ control_field; 

    for (int i = 0; i < DATA_FIELD_SIZE; i++) {
        info_frame[DATA_START_IDX + i] = *packet;
        packet++;
    }

    info_frame[BCC2_IDX] = generate_bcc2(packet);
    info_frame[I_FLAG2_IDX] = FLAG;
}

char assemble_info_frame_ctrl_field(int ns) {
    char control_field = INFO_FRAME_CONTROL;
    if (ns == 1)
        control_field |= BIT(6);
    return control_field;
}

int create_termios_structure(int fd, const char* serialPortName) {
    if (fd < 0) {
        perror(serialPortName);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1) {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));
    
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 30; // Inter-character timer unused
    newtio.c_cc[VMIN] = 0;   // Blocking read until 5 chars received

    tcflush(fd, TCIOFLUSH);
    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");
    return 0;
}
