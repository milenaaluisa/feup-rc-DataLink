#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "transmitter.h"
#include "link_layer.h"
#include "utils.h"
#include "sup_tx_state_machine.h"

int alarm_enabled = 0;
int alarm_count = 0;
extern char control_rcv;

void alarm_handler(int signal) {
    alarm_enabled = 0;
    alarm_count++;
}

int tx_start_transmission(int fd) {
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

    char* set_frame = assemble_supervision_frame(SET_CONTROL);

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            write(fd, set_frame, SUP_FRAME_SIZE);
            printf("SET supervision frame sent\n");
            alarm(3);
            alarm_enabled = 1;
        }
        if (!tx_state_machine(fd)) 
            return 0;
    }
    printf("Transmission failed\n");
    return 1;
}

int tx_stop_transmission(int fd) {
    (void) signal(SIGALRM, alarm_handler);
    printf("New alarm handler set\n");

    char* disc_frame = assemble_supervision_frame(DISC_CONTROL);
    char* ua_frame = assemble_supervision_frame(UA_CONTROL);

    while (alarm_count < 3) {
        if (!alarm_enabled) {
            write(fd, disc_frame, SUP_FRAME_SIZE);
            alarm(3);
            alarm_enabled = 1;
        }
        if (!tx_state_machine(fd) && control_rcv == DISC_CONTROL) {
            printf("DISC supervision frame read\n");

            write(fd, ua_frame, SUP_FRAME_SIZE);
            printf("UA supervision frame sent\n");
            return 0;
        }
    }
    printf("Disconnection failed\n");
    return 1;
}

// TODO: Test
int send_data(int fd, FILE* file_ptr, int file_size) {
    int sequence_number = 0;
    int data_field_size, packet_size;
    unsigned char* file = malloc(file_size * sizeof(unsigned char));

    fread(file, sizeof(unsigned char), file_size, file_ptr);

    for (long i = 0; i < file_size; i += PACKET_DATA_FIELD_SIZE) {
        data_field_size = (i + PACKET_DATA_FIELD_SIZE > file_size)  ? file_size - i : PACKET_DATA_FIELD_SIZE;

        char* data_field = malloc(data_field_size * sizeof(char));
        memcpy(data_field, file + i, data_field_size);

        packet_size = data_field_size + 4;
        char* packet = malloc(packet_size * sizeof(char));

        assemble_data_packet(sequence_number, data_field_size, data_field, packet, packet_size);

        if (llwrite (fd, packet, packet_size) < 0)
            return -1;

        sequence_number = (sequence_number + 1) % DATA_PACKET_MAX_SIZE;
    }
    /*
    int ns = 0, nr;
    int num_successful_packets = 0;
    int reply_from_receiver;
    char control_field = assemble_info_frame_ctrl_field (ns);
    char* info_frame = malloc(INFO_FRAME_SIZE);
    int info_frame_size = assemble_information_frame(control_field, data, info_frame);

    while (num_successful_packets < num_packets) {
        (void) signal(SIGALRM, alarm_handler);
        reply_from_receiver = 0;

        while (alarm_count < 3) {
            if (!alarm_enabled) {
                write(fd, info_frame, info_frame_size);
                printf("Information frame sent\n");
                alarm(3);
                alarm_enabled = 1;
            }

            if (!tx_state_machine(fd)) { 
                nr = control_rcv & BIT(7);

                if ((control_rcv & RR_ACK) == RR_ACK && nr != ns) {
                    num_successful_packets++;
                    data += DATA_FIELD_BYTES;
                    ns = (ns == 0) ? 1 : 0;
                    
                    control_field = assemble_info_frame_ctrl_field(ns);
                    info_frame_size = assemble_information_frame(control_field, data, info_frame);
                }
                reply_from_receiver = 1;
                break;
            }
        }

        if (!reply_from_receiver) {
            printf("Transmission failed\n");
            return 1;
        }
    }*/
    fclose(file_ptr);
    return 0;
}

/*
int main(int argc, char *argv[]) {
    const char *serialPortName = argv[1];
    if (argc < 2) {
        printf("Incorrect program usage\n"
               "Usage: %s <SerialPort>\n"
               "Example: %s /dev/ttyS1\n",
               argv[0],
               argv[0]);
        exit(1);
    }

    // Open serial port device for reading and writing
    int fd = open(serialPortName, O_RDWR | O_NOCTTY);
    if (create_termios_structure(fd, serialPortName)) 
        return 1;

    if (start_transmission(fd)) 
        return 1;

    Testing information frame assembling
    char control_field = BIT(6);
    char* packet = (char*) malloc(DATA_FIELD_BYTES);
    packet = "Helloooooooo world}~";
    char* frame = (char*) malloc(INFO_FRAME_SIZE);
    int frame_size = assemble_information_frame(control_field, packet, frame);
    for (int i = 0; i < frame_size; i++) {
        printf("%08x, %c\n", frame[i], frame[i]);
    }

    
    if (stop_transmission(fd))
        return 1;

    return 0;
}*/
