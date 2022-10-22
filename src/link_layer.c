#include <fcntl.h>

#include "link_layer.h"
#include "utils.h"
#include "transmitter.h"
#include "receiver.h"

int llopen(LinkLayer connection_parameters) {
    int fd = open(connection_parameters.serial_port, O_RDWR | O_NOCTTY);
    if (create_termios_structure(fd, connection_parameters.serial_port)) 
        return -1;

    if (connection_parameters.role == LlRx && rx_start_transmission(fd)) 
        return -1;
    else if (connection_parameters.role == LlTx && tx_start_transmission(fd))
        return -1;
    return fd;
}

int llwrite(int fd, const char* buffer, int buffer_size) {
    // TODO
    return 0;
}

int llread(int fd, char* packet) {
    // TODO
    return 0;
}

int llclose(int fd, LinkLayer connection_parameters) {
    if (connection_parameters.role == LlRx && rx_stop_transmission(fd))
        return -1;
    else if (connection_parameters.role == LlTx && tx_stop_transmission(fd))
        return -1;
    return 0;
}
