// Baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h>
#define BAUDRATE B38400

#define BIT(n) (0x1 << (n))

// size to be read when receiving a transmission
#define BYTE_SIZE 1
// size of supervision frames
#define SUP_FRAME_SIZE 5
// size of information frame data field
#define DATA_FIELD_BYTES 20
#define INFO_FRAME_SIZE (DATA_FIELD_BYTES*2 + 6)

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

// 6th bit of information frame's contol field will be set to 1 depending on Stop and Wait
#define SET_INFO_FRAME_CONTROL BIT(6)
// 7th bit of information frame's contol field will be set to 1 depending on Stop and Wait
#define SET_SUP_FRAME_CONTROL BIT(7)

// used for escaping occurences of flag inside information frame data field
#define ESCAPE 0x7D
#define STF_XOR 0x20
