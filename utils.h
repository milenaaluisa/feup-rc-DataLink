#include <stdio.h>

#include "data-link.h"

char* assemble_supervision_frame(char control_field) {
    char* sup_frame = malloc(SUP_FRAME_SIZE);
    sup_frame[0] = FLAG;
    sup_frame[1] = ADDRESS;
    sup_frame[2] = control_field;
    sup_frame[3] = ADDRESS ^ control_field;
    sup_frame[4] = FLAG;

    return sup_frame;
}

int create_termios_structure (int fd, const char *serialPortName){
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

    return 0;

}