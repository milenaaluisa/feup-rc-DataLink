#ifndef _LINK_LAYER_H_
#define _LINK_LAYER_H_

typedef enum {
    LlTx,
    LlRx,
} LinkLayerRole;

typedef struct {
    char serial_port[50];
    LinkLayerRole role;
} LinkLayer;

#define _POSIX_SOURCE 1 // POSIX compliant source

#define BAUDRATE 9600
#define N_TRIES 3
#define TIMEOUT 4

#define BIT(n) (0x1 << (n))

// size to be read when receiving a transmission
#define BYTE_SIZE 1
// size of supervision frames
#define SUP_FRAME_SIZE 5
// size of information frame data field
#define DATA_FIELD_BYTES 256
#define INFO_FRAME_SIZE (DATA_FIELD_BYTES*2 + 6)
#define PACKET_DATA_FIELD_SIZE 252
#define DATA_PACKET_MAX_SIZE 256

// frame fields indexes
#define FLAG1_IDX 0
#define ADDRESS_IDX 1
#define CONTROL_IDX 2
#define BCC1_IDX 3
#define S_FLAG2_IDX 4
#define DATA_START_IDX 4
// the bcc2 index will vary depending on how many bytes the data field contains after stuffing
#define BCC2_IDX(n) (DATA_START_IDX + n)
#define I_FLAG2_IDX(n) (DATA_START_IDX + n + 1)

// frame fields
#define FLAG 0x7E
#define ADDRESS 0x03
#define SET_CONTROL 0x03
#define DISC_CONTROL 0x0B
#define UA_CONTROL 0x07
#define RR_ACK 0x05
#define REJ_ACK 0x01
#define INFO_FRAME_CONTROL 0x00 // TODO: check if needed

//data packet indexes
#define CTRL_FIELD_IDX 0
#define SEQUENCE_NUM_IDX 1
#define L1_IDX 2
#define L2_IDX 3
#define DATA_FIELD_START_IDX 4

//data packet fields
#define CTRL_FIELD 0x01

// 6th bit of information frame's contol field will be set to 1 depending on Stop and Wait
#define SET_INFO_FRAME_CONTROL BIT(6)
// 7th bit of information frame's contol field will be set to 1 depending on Stop and Wait
#define SET_SUP_FRAME_CONTROL BIT(7)

// used for escaping occurences of flag inside information frame data field
#define ESCAPE 0x7D
#define STF_XOR 0x20

//for control packet 
#define PKT_CTRL_FIELD_IDX 0
#define  TYPE_IDX     1
#define  LENGTH_IDX   2
#define  VALUE_IDX    3

#define TYPE_FILE_SIZE   0
#define TYPE_FILE_NAME   1
#define CTRL_DATA     0x01
#define CTRL_START    0x02
#define CTRL_END      0x03
#define DATA_CTRL_PACK_SIZE 256




// Open a connection using the "port" parameters defined in struct linkLayer.
// Return "1" on success or "-1" on error.
int llopen(LinkLayer connectionParameters);

// Send data in buf with size bufSize.
// Return number of chars written, or "-1" on error.
int llwrite(int fd, char* packet, int packet_size);

// Receive data in packet.
// Return number of chars read, or "-1" on error.
int llread(int fd, char* packet);

// Close previously opened connection.
// Return "1" on success or "-1" on error.
int llclose(int fd, LinkLayer connection_parameters);


#endif // _LINK_LAYER_H_
