#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
#include "obstacle_avoidence.h"
//#include "image_drone.h"


int main(){
	//setbuf(stdout,NULL);
	int n;

	pthread_t obstacle_thread;
	
	//obstacle.sock=targ.shared.sock;
	//n=pthread_create(&cmd_thread,NULL,cmd_thread_func,(void*)&targ);
	n=pthread_create(&obstacle_thread,NULL,obstacle_avoid,NULL);
	//n=pthread_create(&image_thread,NULL,image_drone_func,(void*)&targ);


	pthread_join(obstacle_thread, NULL);
	//pthread_join(image_thread, NULL);

return 0;

}
