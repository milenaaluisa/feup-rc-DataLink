#include <stdio.h>
#ifndef _TRANSMITTER_H_
#define _TRANSMITTER_H_

void alarm_handler(int signal);

int tx_start_transmission(int fd);

int tx_stop_transmission(int fd);

int send_data(int fd, FILE* file_ptr, int file_size);

#endif // _TRANSMITTER_H_