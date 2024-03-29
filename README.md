# RC_T04 - Data Link

> **2022/2023** - 3rd Year, 1st Semester
> 
> **Course** - RC (Redes de Computadores | Computer Networks)
> 
> **Project developed by**
> - Anete Pereira (up202008856)
> - Isabel Amaral (up202006677)
> - Milena Gouveia (up202008862)

### Project description
The goal of this project was to develop a data link protocol for transferring files between two systems connected through a serial cable.

### Project Structure
- **bin/**: Compiled binaries
- **src/**: Source code for the implementation of the link-layer and application layer protocols
- **include/**: Header files of the link-layer and application layer protocols
- **cable/**: Virtual cable program to help test the serial port
- **main.c**: Main file
- **Makefile**: Makefile to build the project and run the application
- **penguin.gif**: Example file to be sent through the serial port

### Instructions to Run the Project
**Step 1:** Compile the application and the virtual cable program using the provided Makefile.<br>
**Step 2:** Run the virtual cable program (either by running the executable manually or using the Makefile target):
```bash
./bin/cable_app
make run_cable
```
**Step 3:** Test the protocol without cable disconnections and noise <br>
- Run the receiver (either by running the executable manually or using the Makefile target):
```bash
	./bin/main /dev/ttyS11 rx penguin-received.gif
	make run_tx
```

- Run the transmitter (either by running the executable manually or using the Makefile target):
```bash
	./bin/main /dev/ttyS10 tx penguin.gif
	make run_rx
```

- Check if the file received matches the file sent, using the diff Linux command or using the Makefile target:
```bash
	diff -s penguin.gif penguin-received.gif
	make check_files
```

**Step 4:** Test the protocol with cable disconnections and noise <br>
- Run receiver and transmitter again
- Quickly move to the cable program console and press 0 for unplugging the cable, 2 to add noise, and 1 to normal
- Check if the file received matches the file sent, even with cable disconnections or with noise
