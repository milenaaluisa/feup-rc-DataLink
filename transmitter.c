#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

// Baudrate settings are defined in <asm/termbits.h>, which is
// included by <termios.h>
#define BAUDRATE B38400

#define SUP_FRAME_SIZE 5
#define FLAG 0x7E
#define ADDRESS 0x03
#define SET_CONTROL 0x03
#define UA_CONTROL 0x07

/*
typedef struct supervision_frame {
    char start_flag;
    char address;
    char control;
    char block_check;
    char end_flag;
};*/

char* assemble_supervision_frame() {
    char* set_frame = malloc(SUP_FRAME_SIZE);
    set_frame[0] = FLAG;
    set_frame[1] = ADDRESS;
    set_frame[2] = SET_CONTROL;
    set_frame[3] = ADDRESS ^ SET_CONTROL;
    set_frame[4] = FLAG;

    return set_frame;
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

    char* set_frame = assemble_supervision_frame();
    write(fd, set_frame, SUP_FRAME_SIZE);
    printf("Supervision frame sent\n");

    return 0;
}
