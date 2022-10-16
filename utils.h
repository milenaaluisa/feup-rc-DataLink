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