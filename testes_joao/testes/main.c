#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
#include "obstacle_avoidence.h"
#include "image_drone.h"
//#include "image_drone.h"
//#include "cmd.h"
//#include "main.h"
char send_temp[MAX_SIZE];
//float val[2];
//pthread_t obstacle_thread;
ajustments struct_ajustments;


int main(){
	//setbuf(stdout,NULL);
	int n;
	pthread_t cmd_thread;
	pthread_t image_thread;
	pthread_t obstacle_thread;

	int seq=0;
	
	//drone_com obstacle;
	char buffer[MAX_SIZE];
	

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
	//n=pthread_create(&cmd_thread,NULL,cmd_thread_func,(void*)&targ);
	//n=pthread_create(&obstacle_thread,NULL,obstacle_avoid,NULL);
	//n=pthread_create(&image_thread,NULL,image_drone_func,(void*)&targ);






	obstacle_avoid();

	//CRIA INTERRUPT TEMPORAL E ESTABELECE A FUNÇÃO avoidObstacleHandler
	//COMO HANDLER DO INTERRUPT
	struct itimerval itv;
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = avoidObstacleHandler;

	if (sigaction(SIGALRM, &sa, NULL) == -1)
		perror("sigaction\n");
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 60000;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 60000;
	
	if (setitimer(ITIMER_REAL, &itv, NULL) == -1)
		perror("setitimer\n");








	while(1)
	{

	
	//	obstacle_avoid();
		//CREAT OBSTACLE AVOIDENCE THREAD
		printf("hello\n");
		

		memset(buffer,0,MAX_SIZE);
		if(struct_ajustments.NS || struct_ajustments.EW)
			snprintf(buffer,1024,"ET*PCMD=,%u,%d,%d,0,0\r",seq,struct_ajustments.EW_ajustment,struct_ajustments.NS_ajustment);
		else
			snprintf(buffer,1024,"ET*PCMD=,%u%s\r",seq,send_temp);



		if (sendto(targ.shared.sock,buffer, strlen(buffer) , 0 , (struct sockaddr *) &(targ.si_other), sizeof(struct sockaddr_in))==-1){
	        die("sendto()");
	    }
	    else
	    	printf("enviado\n");

		usleep(40000);
	//	sleep(100);

	}

	//n = pthread_create(&obstacle_thread, NULL, (void*)obstacle_avoid, (void*)&targ);

	if(n != 0){
		printf("COULDN'T CREAT THREAD\n");
		return -1;
	}

	//WAIT ON THREADS
	//pthread_join(cmd_thread,NULL);
	//pthread_join(obstacle_thread, NULL);
	//pthread_join(image_thread, NULL);

return 0;

}
