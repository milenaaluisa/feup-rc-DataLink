#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "application_layer.h"
#include "link_layer.h"
#include "utils.h"

// TODO: Test
int send_data(int fd, char* data, int file_size) {
    int sequence_number = 0;
    int data_field_size, packet_size;
    char data_field[DATA_FIELD_BYTES];
    char packet[DATA_FIELD_BYTES + 4];

    for (long i = 0; i < file_size; i += PACKET_DATA_FIELD_SIZE) {
        data_field_size = (i + PACKET_DATA_FIELD_SIZE > file_size)  ? file_size - i : PACKET_DATA_FIELD_SIZE;
        memcpy(data_field, data + i, data_field_size);

        packet_size = data_field_size + 4;
        assemble_data_packet(sequence_number, data_field, data_field_size, packet);
        if (llwrite(fd, packet, packet_size))
            return 1;

        sequence_number = (sequence_number + 1) % DATA_PACKET_MAX_SIZE;
    }
    return 0;
}

int send_file(int fd, const char* filename) {
    FILE* fptr;
    if (!(fptr = fopen(filename, "r")))
        return 1;

    fseek(fptr, 0, SEEK_END);
    long file_size = ftell(fptr);
    rewind(fptr);

    char* data = (char*) malloc(file_size);
    if (fread(data, sizeof(char), file_size, fptr) < file_size)
        return 1;

    
    if (send_control_packet(fd, CTRL_START, file_size, filename)) 
        return 1;
    if (send_data(fd, data, file_size))
        return 1;
    if (send_control_packet(fd, CTRL_END, file_size, filename)) 
        return 1;

    if (fclose(fptr))
        return 1;
    return 0;
}

int receive_file(int fd) {
    char* src_filename = NULL; 
    long file_size;

    if (receive_control_packet(fd, CTRL_START, &file_size, src_filename))
        return 1;
    printf("%ld\n", file_size);
    printf("%s\n", src_filename);
    
    char* data = (char*) malloc(file_size);
    char* data_ptr = data;
    char* packet = (char*) malloc(DATA_CTRL_PACK_SIZE); 
    int packet_size;
    memset(packet, 0, DATA_CTRL_PACK_SIZE); 

    while (packet[0] != CTRL_END) { 
        llread(fd, packet);
        packet_size = DATA_CTRL_PACK_SIZE * packet[2] + packet[3]; 
        memcpy(data_ptr, packet + 4, packet_size);
        data_ptr += packet_size;
    }
    
    char* dest_filename = (char*) malloc(sizeof("received_") + sizeof(src_filename) - 1);
    strcpy(dest_filename, "received_");
    strcat(dest_filename, src_filename);

    FILE* fptr;
    if (!(fptr = fopen(dest_filename, "w")))
        return 1;
    if (fwrite(data, sizeof(char), file_size, fptr) < file_size)
        return 1;
    if (fclose(fptr))
        return 1;
    return 0;
}

int application_layer(const char *serial_port, const char *role, const char *filename) {
    LinkLayer connection_parameters;
    strcpy(connection_parameters.serial_port, serial_port);

    LinkLayerRole link_layer_role;
    if (!strcmp(role, "tx"))
        link_layer_role = LlTx;
    else
        link_layer_role = LlRx;
    connection_parameters.role = link_layer_role;        

    int fd; 
    if ((fd = llopen(connection_parameters)) < 0)
        return 1;

    if (link_layer_role == LlTx && send_file(fd, filename))
        return 1;
    else if (link_layer_role == LlRx && receive_file(fd))
        return 1;

    if (llclose(fd, connection_parameters) < 0)
        return 1;
    return 0;
}
