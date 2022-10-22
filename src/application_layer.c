#include <stdio.h>
#include <string.h>

#include "application_layer.h"
#include "link_layer.h"

int send_file(int fd, const char* filename) {
    FILE* fptr;
    if (!(fptr = fopen(filename, "r")))
        return 1;

    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    rewind(fptr);

    // TODO
    // send control packet
    // send data
    // send control packet

    if (fclose(fptr))
        return 1;
    return 0;
}

int applicationLayer(const char *serial_port, const char *role, const char *filename) {
    LinkLayer connection_parameters;
    strcpy(connection_parameters.serial_port, serial_port);

    LinkLayerRole link_layer_role;
    if (!strcmp(role, "tx"))
        link_layer_role = LlTx;
    else
        link_layer_role = LlRx;
    connection_parameters.role = link_layer_role;        

    int fd; 
    if ((fd = llopen(connection_parameters)) < 0)
        return 1;
    if (link_layer_role == LlTx && send_file(fd, filename))
        return 1;
    if (llclose(fd, connection_parameters) < 0)
        return 1;
    return 0;
}
