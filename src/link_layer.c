#include <fcntl.h>

#include "link_layer.h"
#include "utils.h"
#include "transmitter.h"
#include "receiver.h"

int llopen(LinkLayer connection_parameters) {
    int fd = open(connection_parameters.serial_port, O_RDWR | O_NOCTTY);
    if (create_termios_structure(fd, connection_parameters.serial_port)) 
        return -1;

    if (connection_parameters.role == LlRx)
        rx_start_transmission(fd);
    else
        tx_start_transmission(fd);
    return fd;
}

int llwrite(const unsigned char *buf, int bufSize) {
    // TODO
    return 0;
}

int llread(unsigned char *packet) {
    // TODO
    return 0;
}

int llclose(int showStatistics) {
    // TODO
    return 1;
}
