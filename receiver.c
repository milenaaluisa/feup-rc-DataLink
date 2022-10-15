#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "data-link.h"

enum State {START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP};
enum State state = START;

char control_rcv[BYTE_SIZE];

void start_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = FLAG_RCV;
}

void flag_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == ADDRESS)
        state = A_RCV;
    else if (byte_rcv != FLAG)
        state = START;
}

void a_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == SET_CONTROL || byte_rcv == DISC_CONTROL){
        state = C_RCV;
        control_rcv[0] = byte_rcv;
    }
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void c_rcv_transition_check(char byte_rcv) {
    if (byte_rcv == (ADDRESS ^ SET_CONTROL))
        state = BCC_OK;
    else if (byte_rcv == FLAG)
        state = FLAG_RCV;
    else
        state = START;
}

void bcc_ok_transition_check(char byte_rcv) {
    if (byte_rcv == FLAG)
        state = STOP;
    else
        state = START;
}

int send_back_disc_frame(int fd) {
    char* disc_frame = malloc(SUP_FRAME_SIZE);
    char* ua_frame_rcv = malloc(SUP_FRAME_SIZE);

    disc_frame[0] = FLAG;
    disc_frame[1] = ADDRESS;
    disc_frame[2] = DISC_CONTROL;
    disc_frame[3] = ADDRESS ^ DISC_CONTROL;
    disc_frame[4] = FLAG;

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

    char byte_rcv[BYTE_SIZE];
    while (state != STOP) {
        read(fd, byte_rcv, BYTE_SIZE);
        printf("%08x\n", byte_rcv[0]);

        switch (state) {
        case START:
            start_transition_check(byte_rcv[0]); break;
        case FLAG_RCV:
            flag_rcv_transition_check(byte_rcv[0]); break;
        case A_RCV:
            a_rcv_transition_check(byte_rcv[0]); break;
        case C_RCV:
            c_rcv_transition_check(byte_rcv[0]); break;
        case BCC_OK:
            bcc_ok_transition_check(byte_rcv[0]); break;
        }
    }
    
    printf("Supervision frame read\n");

    /*
    if (control_rcv[0] == DISC_CONTROL){
        if (send_back_disc_frame(fd) != 0) 
            printf("Disconnection failed. \n");
            return 1;
    }*/
        
    return 0;
}
