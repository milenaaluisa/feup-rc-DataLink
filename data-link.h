// Baudrate settings are defined in <asm/termbits.h>, which is included by <termios.h>
#define BAUDRATE B38400

// size to be read when receiving a transmission
#define BYTE_SIZE 1
// size of supervision frames
#define SUP_FRAME_SIZE 5
// size of information frame data field
#define DATA_FIELD_SIZE 20

// frame fields
#define FLAG 0x7E
#define ADDRESS 0x03
#define SET_CONTROL 0x03
#define DISC_CONTROL 0x0B
#define UA_CONTROL 0x07

// used for escaping occurences of flag inside information frame data field
#define ESCAPE 0x7D
#define STF_XOR 0x20
