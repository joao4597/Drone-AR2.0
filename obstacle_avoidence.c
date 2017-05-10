#include "serial_comm.h"

int main(){

	int i;

	i = serialport_init();

	if(i == -1){
		printf("ERROR OEPNING SERIALPORT\n");
		exit(0);
	}

	serialport_read();

	return 0;
}