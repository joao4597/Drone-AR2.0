#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
//#include "obstacle_avoidence.h"
#include "image_drone.h"
//#include "image_drone.h"
//#include "cmd.h"
//#include "main.h"
char send_temp[MAX_SIZE];
//float val[2];
pthread_t obstacle_thread;

int main(){
	//setbuf(stdout,NULL);
	int n;
	pthread_t cmd_thread;
	pthread_t image_thread;
	int seq=0;
	
	//drone_com obstacle;
	
	pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t lock_search=PTHREAD_MUTEX_INITIALIZER;

	th_arg targ;
	
	//obstacle.seq=&seq;
	//obstacle.lock=&lock;
	
	targ.shared.seq=&seq;
	targ.shared.lock=&lock;
	targ.lock_pvt=&lock_search;

	open_connection(&(targ.shared.sock),&(targ.si_other),PORT1);
	
	//obstacle.sock=targ.shared.sock;
	
	n=pthread_create(&cmd_thread,NULL,cmd_thread_func,(void*)&targ);
	
	//CREAT OBSTACLE AVOIDENCE THREAD
	printf("hello\n");
	n=pthread_create(&image_thread,NULL,image_drone_func,(void*)&targ);

	//n = pthread_create(&obstacle_thread, NULL, (void*)obstacle_avoid, (void*)&targ);

	if(n != 0){
		printf("COULDN'T CREAT THREAD\n");
		return -1;
	}

	//WAIT ON THREADS
	pthread_join(cmd_thread,NULL);
	//pthread_join(obstacle_thread, NULL);
	pthread_join(image_thread, NULL);

return 0;

}