#ifndef _APPLICATION_LAYER_H_
#define _APPLICATION_LAYER_H_

// Application layer main function.
// Arguments:
//   serialPort: Serial port name (e.g., /dev/ttyS0).
//   role: Application role {"tx", "rx"}.
//   filename: Name of the file to send / receive.
void applicationLayer(const char *serialPort, const char *role, const char *filename);

#endif // _APPLICATION_LAYER_H_
