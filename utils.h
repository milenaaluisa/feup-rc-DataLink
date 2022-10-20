#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>

#include "data-link.h"


// Função para calcular bcc2
char calculate_bcc2(const char* data_rcv){
    char bcc2 = data_rcv[0];
    for (int i = 1; i < DATA_FIELD_SIZE; i++){
        bcc2 = (bcc2 ^ data_rcv[i]);
    }   
    return bcc2;
}

char* assemble_supervision_frame(char control_field) {
    char* sup_frame = malloc(SUP_FRAME_SIZE);
    sup_frame[0] = FLAG;
    sup_frame[1] = ADDRESS;
    sup_frame[2] = control_field;
    sup_frame[3] = ADDRESS ^ control_field;
    sup_frame[4] = FLAG;

    return sup_frame;
}

char* assemble_information_frame(char control_field, const char* packet) {
    char* info_frame = malloc(INFO_FRAME_SIZE);
    info_frame[0] = FLAG;
    info_frame[1] = ADDRESS;
    info_frame[2] = control_field;
    info_frame[3] = ADDRESS ^ control_field; 

    for (int i = 0; i < DATA_FIELD_SIZE; i++) {
        info_frame[4 + i] = *packet;
        packet++;
    }

    info_frame[24] = calculate_bcc2(packet); // changed
    info_frame[25] = FLAG;
}

char assemble_info_frame_ctrl_field (int ns) {
    char control_field = INFO_FRAME_CONTROL;
    if (ns == 1)
        control_field = control_field | BIT(6);

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
