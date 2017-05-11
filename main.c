#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
#include "obstacle_avoidence.h"

pthread_t obstacle_thread;

int main(){
	int n;

	//CREAT OBSTACLE AVOIDENCE THREAD
	n = pthread_create(&obstacle_thread, NULL, (void*)obstacle_avoid, NULL);
	if(n != 0){
		printf("COULDN'T CREAT THREAD\n");
		return -1;
	}

	//WAIT ON THREADS
	pthread_join(obstacle_thread, NULL);
}