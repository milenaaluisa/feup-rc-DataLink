#include <stdio.h>
#include <stdlib.h>

#include "application_layer.h"

// Arguments:
//   $1: /dev/ttySxx
//   $2: tx | rx
//   $3: filename (optional)
int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s /dev/ttySxx tx|rx filename\n", argv[0]);
        exit(1);
    }

    const char *serial_port = argv[1];
    const char *role = argv[2];
    const char *filename = argv[3];

    printf("Starting link-layer protocol application\n"
           "  - Serial port: %s\n"
           "  - Role: %s\n"
           "  - Filename: %s\n",
           serial_port,
           role,
           filename);

    if (applicationLayer(serial_port, role, filename))
        return 1;
    return 0;
}
