#ifndef _UTILS_H_
#define _UTILS_H_

int stuffing(char* data, char* stuffed_data, int data_size);

char generate_bcc2(const char* data_rcv, int data_size);

int send_control_packet(int fd, unsigned ctrl_control_field, long file_size, const char* file_name);

char* receive_control_packet(int fd, unsigned char control_field, long* file_size);

void assemble_data_packet(int sequence_number, char* data, int data_size, char* packet);

char* assemble_supervision_frame(char control_field);

char* assemble_information_frame(char control_field, char* buffer, int buffer_size, int* info_frame_size) ;

char assemble_info_frame_ctrl_field(int ns);

char assemble_rr_frame_ctrl_field(int ns);

char assemble_rej_frame_ctrl_field(int ns);

int create_termios_structure(int fd, const char* serialPortName);

#endif // _UTILS_H_
