#include "serial_comm.h"
#include <sys/types.h>
#include <sys/syscall.h>

void obstacle_avoid(){

	int i;
	
	//SET THREAD PRIORITY
	struct sched_param sched_param_obstacle;
	sched_param_obstacle.sched_priority = 50;


	//OPEN USB0
	i = serialport_init();

	//GET THREAD pid_t
	pid_t x = syscall(__NR_gettid);


	//SET THREAD PRIORITY
	sched_setscheduler(x, SCHED_FIFO, &sched_param_obstacle);

	if(i == -1){
		printf("ERROR OEPNING SERIALPORT\n");
		exit(0);
	}


	while(1)
		serialport_read();

}