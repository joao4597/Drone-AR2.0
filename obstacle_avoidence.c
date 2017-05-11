#include "serial_comm.h"
#include "obstacle_avoidence.h"
#include <sys/types.h>
#include <sys/syscall.h>
#include <assert.h>
//#include "cmd.h"

distance struct_distances;
float slop = (SPEED_AT_DIS_HIGH - SPEED_AT_DIS_LOW) / (D_HIGH_LIMIT - D_LOW_LIMIT);

void obstacle_avoid(void *obstacle){

	int i;
	char buff[1024];
	float ajustment_right, ajustment_left, ajustment_front;
	assert(sizeof(int)==sizeof(float));
	//SET THREAD PRIORITY
	struct sched_param sched_param_obstacle;
	sched_param_obstacle.sched_priority = 50;
	th_arg *targ = obstacle;


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


	while(1){

		serialport_read();
		printf("front-> %f\nback-> %f\nleft-> %f\nright-> %f\n\n\n", struct_distances.front, struct_distances.back, struct_distances.left, struct_distances.right);
		if(struct_distances.front < D_LOW_LIMIT || struct_distances.left < D_LOW_LIMIT || struct_distances.right < D_LOW_LIMIT){


			while(struct_distances.front < D_HIGH_LIMIT || struct_distances.left < D_HIGH_LIMIT || struct_distances.right < D_HIGH_LIMIT){

				if(struct_distances.front < D_HIGH_LIMIT)
					ajustment_front = -((struct_distances.front * slop) + 1);
				else
					ajustment_front = 0;

				//SEND THIS IS THERE IS AN OBSTACLE ON THE LEFT SIDE
				if(struct_distances.left < D_HIGH_LIMIT){
					
					ajustment_left = (struct_distances.left * slop) + 1;
					printf("move front->%f\nmoveright%f\n\n\n", ajustment_front, ajustment_left);
					pthread_mutex_lock(targ->shared.lock);
					snprintf(buff,1024,"AT*PCMD=%u,1,%d,%d,%d,%d\r",*(targ->shared.seq),*(int*)(&(ajustment_left)),*(int*)(&(ajustment_front)), 0, 0);
					(*(targ->shared.seq))++;
					pthread_mutex_unlock(targ->shared.lock);


					if (sendto(targ->shared.sock, buff, strlen(buff) , 0 , (struct sockaddr *) &(targ->si_other), sizeof(targ->si_other))==-1){
						die("sendto()");
					}
					else
						printf("enviado\n");

					//(*(targ->shared.seq))++;

				//SEND THIS STRING IF THERE IS AN OBJECT ON THE RIGHT SIDE
				}else if(struct_distances.right < D_HIGH_LIMIT){

					ajustment_right = -((struct_distances.right * slop) + 1);
					printf("move front->%f\nmoveleft%f\n\n\n", ajustment_front, ajustment_right);
					pthread_mutex_lock(targ->shared.lock);
					
					snprintf(buff,1024,"AT*PCMD=%u,1,%d,%d,%d,%d\r",*(targ->shared.seq),*(int*)(&(ajustment_right)),*(int*)(&(ajustment_front)), 0, 0);
					(*(targ->shared.seq))++;
					
					pthread_mutex_unlock(targ->shared.lock);

					if (sendto(targ->shared.sock, buff, strlen(buff) , 0 , (struct sockaddr *) &(targ->si_other), sizeof(targ->si_other))==-1){
						die("sendto()");
					}
					else
						printf("enviado\n");

				}
				serialport_read();
				usleep(1000);
			}
			pthread_mutex_lock(targ->shared.lock);
			snprintf(buff,1024,"AT*PCMD=%u,1,%d,%d,%d,%d\r",*(targ->shared.seq) , 0, 0, 0, 0);
			(*(targ->shared.seq))++;
			pthread_mutex_unlock(targ->shared.lock);
			if (sendto(targ->shared.sock, buff, strlen(buff) , 0 , (struct sockaddr *) &(targ->si_other), sizeof(struct sockaddr_in))==-1){
				die("sendto()");
			}
			else
				printf("enviado\n");
			
		}

		usleep(1000);

	}
}