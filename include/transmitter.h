#ifndef _TRANSMITTER_H_
#define _TRANSMITTER_H_

void alarm_handler(int signal);

int start_transmission(int fd);

int stop_transmission(int fd);

int data_transfer(int fd, char* data, int num_packets);

#endif // _TRANSMITTER_H_