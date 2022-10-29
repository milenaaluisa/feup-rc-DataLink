#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

#define DATA_PACKET_MAX_SIZE 256
#define PACKET_DATA_FIELD_SIZE 252

// data packet indexes
#define CTRL_FIELD_IDX 0
#define SEQUENCE_NUM_IDX 1
#define L2_IDX 2
#define L1_IDX 3
#define DATA_FIELD_START_IDX 4

// control packet indexes 
#define PKT_CTRL_FIELD_IDX 0
#define TYPE1_IDX 1
#define LENGTH1_IDX 2
#define VALUE1_IDX 3

#define TYPE_FILE_SIZE 0
#define TYPE_FILE_NAME 1
#define CTRL_DATA 1
#define CTRL_START 2
#define CTRL_END 3
#define DATA_CTRL_PACK_SIZE 256

int send_data(int fd, unsigned char* data, int file_size);

int send_file(int fd, const char* filename);

int receive_file(int fd);

// Application layer main function.
// Arguments:
//   serialPort: Serial port name (e.g., /dev/ttyS0).
//   role: Application role {"tx", "rx"}.
//   filename: Name of the file to send / receive.
// Return "0" on success or "1" on error.
int application_layer(const char *serial_port, const char *role, const char *filename);

#endif // _APPLICATION_LAYER_H_
