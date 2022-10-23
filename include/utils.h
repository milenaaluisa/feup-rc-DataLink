#ifndef _UTILS_H_
#define _UTILS_H_

int stuffing(char* data, char* stuffed_data);

char generate_bcc2(const char* data_rcv);

char* assemble_supervision_frame(char control_field);

int assemble_information_frame(char control_field, char* packet, char* info_frame);

char assemble_info_frame_ctrl_field(int ns);

char assemble_rr_frame_ctrl_field(int ns);

char assemble_rej_frame_ctrl_field(int ns);

int create_termios_structure(int fd, const char* serialPortName);

#endif // _UTILS_H_
