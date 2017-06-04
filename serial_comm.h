#include <stdio.h>    /* Standard input/output definitions */
#include <stdlib.h> 
#include <stdint.h>   /* Standard types */
#include <string.h>   /* String function definitions */
#include <unistd.h>   /* UNIX standard function definitions */
#include <fcntl.h>    /* File control definitions */
#include <errno.h>    /* Error number definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <sys/ioctl.h>
#include <getopt.h>
#include <sched.h>



/*
	_distances to obstacles on all directions
*/
typedef struct distances{
	float front;
	float back;
	float left;
	float right;
}distance;

extern distance struct_distances;


/*
	_define port name
	_define baudrate
*/
#define PORT_NAME "/dev/ttyACM0"
#define BAUD B9600



/*
	_uses PORT_NAME (#define) as name of the serial port (e.g. "/dev/ttyUSB0")
		and a baud rate (bps) and connects to that port at that speed and 8N1.
	_opens the port in fully raw mode so you can send binary data.
	_clears buffer
	_returns valid fd, or -1 on error
*/
int serialport_init();

/*
	_read distances obtained by the arduino
*/
int serialport_read();