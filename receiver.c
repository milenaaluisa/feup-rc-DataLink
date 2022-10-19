#include <fcntl.h>

#include "data-link.h"
#include "utils.h"
#include "receiver-state-machine.h"

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

/* Testing destuffing
int receive_inf_frame(int fd) {
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
}*/

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
    
    /* Testing destuffing
    receive_inf_frame(fd);
    printf("Information frame received\n"); */

    char* ua_frame = assemble_supervision_frame(UA_CONTROL);
    write(fd, ua_frame, SUP_FRAME_SIZE);
    printf("Acknowledgement frame sent\n");

    /*
    if (control_rcv[0] == DISC_CONTROL){
        if (send_back_disc_frame(fd) != 0) 
            printf("Disconnection failed. \n");
            return 1;
    }*/

    return 0;
}
