#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
#include "obstacle_avoidence.h"
#include "cmd.h"
//#include "main.h"


pthread_t obstacle_thread;

int main(){
	int n;
	pthread_t cmd_thread;
	int seq=0;
	drone_com obstacle;
	pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

	th_arg targ;
	
	obstacle.seq=&seq;
	obstacle.lock=&lock;
	
	targ.shared.seq=&seq;
	targ.shared.lock=&lock;

	open_connection(&(targ.shared.sock),&(targ.si_other),PORT1);
	obstacle.sock=targ.shared.sock;
	
	n=pthread_create(&cmd_thread,NULL,cmd_thread_func,(void*)&targ);
	
	//CREAT OBSTACLE AVOIDENCE THREAD
	
	/*n = pthread_create(&obstacle_thread, NULL, (void*)obstacle_avoid, NULL);
	if(n != 0){
		printf("COULDN'T CREAT THREAD\n");
		return -1;
	}*/

	//WAIT ON THREADS
	pthread_join(cmd_thread,NULL);
	//pthread_join(obstacle_thread, NULL);
}