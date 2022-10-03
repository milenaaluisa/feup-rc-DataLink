#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#define SUP_FRAME_SIZE 5
#define FLAG 0x7E
#define ADDRESS 0x03
#define SET_CONTROL 0x03

typedef struct supervision_frame {
    char start_flag;
    char address;
    char control;
    char block_check;
    char end_flag;
}

char[SUP_FRAME_SIZE] assemble_supervision_frame() {
    char set_frame[SUP_FRAME_SIZE];
    set_frame[0] = FLAG;
    set_frame[1] = ADDRESS;
    set_frame[2] = SET_CONTROL;
    set_frame[3] = ADDRESS ^ SET_CONTROL;
    set_frame[4] = FLAG;

    return set_frame;
}

int main(int argc, char *argv[]) {
    char set_frame[SUP_FRAME_SIZE] = assemble_supervision_frame();

    return 0;
}
