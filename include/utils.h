#ifndef _UTILS_H_
#define _UTILS_H_

int stuffing(unsigned char* data, unsigned char* stuffed_data, int data_size);

unsigned char generate_bcc2(const unsigned char* data_rcv, int data_size);

int send_control_packet(int fd, unsigned ctrl_control_field, long file_size, const unsigned char* file_name);

unsigned char* receive_control_packet(int fd, unsigned char control_field, long* file_size);

void assemble_data_packet(int sequence_number, unsigned char* data, int data_size, unsigned char* packet);

unsigned char* assemble_supervision_frame(unsigned char control_field);

unsigned char* assemble_information_frame(unsigned char control_field, unsigned char* buffer, int buffer_size, int* info_frame_size) ;

unsigned char assemble_info_frame_ctrl_field(int ns);

unsigned char assemble_rr_frame_ctrl_field(int ns);

unsigned char assemble_rej_frame_ctrl_field(int ns);

int create_termios_structure(int fd, const char* serialPortName);

#endif // _UTILS_H_
