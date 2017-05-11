#include "serial_comm.h"
#include "obstacle_avoidence.h"
#include <sys/types.h>
#include <sys/syscall.h>

distance struct_distances;
float slop = (SPEED_AT_DIS_HIGH - SPEED_AT_DIS_LOW) / (D_HIGH_LIMIT - D_LOW_LIMIT);

void obstacle_avoid(){

	int i;
	float ajustment_right, ajustment_left, ajustment_front;
	
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


	while(1){

		serialport_read();
		printf("front-> %f\nback-> %f\nleft-> %f\nright-> %f\n\n\n", struct_distances.front, struct_distances.back, struct_distances.left, struct_distances.right);
		if(struct_distances.front < D_LOW_LIMIT || struct_distances.left < D_LOW_LIMIT || struct_distances.right < D_LOW_LIMIT){
			

			
			//printf("AVOID!!\n");
			

			while(struct_distances.front < D_HIGH_LIMIT || struct_distances.left < D_HIGH_LIMIT || struct_distances.right < D_HIGH_LIMIT){
				//snprintf(buff,1024,"AT*PCMD=%u,1,%d,%d,%d,%d\r",*seq,*(int*)(&(analog[0].x)),*(int*)(&(analog[0].y)), 0, 0);
				if(struct_distances.front < D_HIGH_LIMIT)
					ajustment_front = -((struct_distances.front * slop) + 1);
				else
					ajustment_front = 0;

				//SEND THIS IS THERE IS AN OBSTACLE ON THE LEFT SIDE
				if(struct_distances.left < D_HIGH_LIMIT){
					
					ajustment_left = (struct_distances.left * slop) + 1;
					printf("move front->%f\nmoveright%f\n\n\n", ajustment_front, ajustment_left);
					/*if (sendto(sock,buff, strlen(buff) , 0 , (struct sockaddr *) &si_other, sizeof(si_other))==-1){
						die("sendto()");
					}
					else
						printf("enviado\n");*/

				//SEND THIS STRING IF THERE IS AN OBJECT ON THE RIGHT SIDE
				}else if(struct_distances.right < D_HIGH_LIMIT){
					ajustment_front = (struct_distances.front * slop) + 1;
					ajustment_right = -((struct_distances.right * slop) + 1);
					printf("move front->%f\nmoveleft%f\n\n\n", ajustment_front, ajustment_right);
					/*if (sendto(sock,buff, strlen(buff) , 0 , (struct sockaddr *) &si_other, sizeof(si_other))==-1){
						die("sendto()");
					}
					else
						printf("enviado\n");*/
				}
				serialport_read();
			}

			/*snprintf(buff,1024,"AT*PCMD=%u,1,%d,%d,%d,%d\r",0 , 0, 0, 0, 0);
			if (sendto(sock,buff, strlen(buff) , 0 , (struct sockaddr *) &si_other, sizeof(si_other))==-1){
				die("sendto()");
			}
			else
				printf("enviado\n");*/
		}
	}

}