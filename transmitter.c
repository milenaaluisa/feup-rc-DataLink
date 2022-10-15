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

int alarm_enabled = 0;
int alarm_count = 0;

void alarm_handler(int signal) {
    alarm_enabled = 0;
    alarm_count++;
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
    
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

    char* set_frame = assemble_supervision_frame(SET_CONTROL);
    char ua_frame[SUP_FRAME_SIZE];

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            write(fd, set_frame, SUP_FRAME_SIZE);
            printf("Supervision frame sent\n");
            alarm(3);
            alarm_enabled = 1;

            /* testing stuffing
            char* data = (char*) malloc(DATA_FIELD_SIZE);
            data = "Hello world}~ooooo"; // } is 0x7d, ~ is 0x7e, becomes: }] and }^
            printf("%s\n", stuffing(data)); */

        }
        if (read(fd, ua_frame, SUP_FRAME_SIZE)) {
            for (int i = 0; i < SUP_FRAME_SIZE; i++)
                printf("%08x\n", ua_frame[i]);
            printf("Acknowledgement frame read\n");
            return 0;
        }
    }
    printf("Transmission failed\n");
    return 1;
}
