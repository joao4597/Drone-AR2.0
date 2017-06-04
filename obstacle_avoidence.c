#include "serial_comm.h"
#include "obstacle_avoidence.h"
#include <sys/types.h>
#include <sys/syscall.h>
#include <assert.h>
#include <signal.h>
//#include "cmd.h"

void avoidObstacleHandler();
void accessDanger();
void calculateAjustment();
void calculateTaskTimes();
void timeBetweenTaskCalls();

float ajustment_right = 0, ajustment_left = 0, ajustment_front = 0;
int danger_front = 0, danger_right = 0, danger_left = 0;

distance struct_distances;
float slop = (SPEED_AT_DIS_HIGH - SPEED_AT_DIS_LOW) / (D_HIGH_LIMIT - D_LOW_LIMIT);

/*void obstacle_avoid(void *obstacle){*/
int main(){

	int i;
	char buff[1024];
	assert(sizeof(int)==sizeof(float));


	//OPEN USB0
	i = serialport_init();

	if(i == -1){
		printf("ERROR OEPNING SERIALPORT\n");
		exit(0);
	}

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
	itv.it_value.tv_usec = 90000;
	itv.it_interval.tv_sec = 0;
	itv.it_interval.tv_usec = 90000;
	
	if (setitimer(ITIMER_REAL, &itv, NULL) == -1)
		perror("setitimer\n");
	
	//SLEEP FOR EVER ATÉ RECEBER SIGKILL
	while(1){
		sleep(5000);
	}
}

void avoidObstacleHandler(int sig){
	//CHAMADA PELOS INTERRUPTS TEMPORAIS CRIADOS
	//CALCULA TEMPO ENTRE CHAMADAS E TEMPO TOTAL
	//QUE DEMOROU A EXECUÇÃO DA TAREFA
	timeBetweenTaskCalls();
	calculateTaskTimes();
	serialport_read();
	accessDanger();
	calculateAjustment();
	calculateTaskTimes();
}

void accessDanger(){
	//VERIFICAR CADA UMA DAS DIREÇOES, DIRECÇOES COM 1 TÊM OBSTACULO
	//ESTA FUNÇÃO SERVE PARA ASSINALAR PERIGO E PARA LIMPAR CASO O
	//OBSTACULO DEIXE DE EXISTIR
	if(danger_front == 1){
		if(struct_distances.front > D_HIGH_LIMIT)
			danger_front = 0;
	}else
		if(struct_distances.front < D_LOW_LIMIT)
			danger_front = 1;

	if(danger_left == 1){
		if(struct_distances.left > D_HIGH_LIMIT)
			danger_left = 0;
	}else
		if(struct_distances.left < D_LOW_LIMIT)
			danger_left = 1;

	if(danger_right == 1){
		if(struct_distances.right > D_HIGH_LIMIT)
			danger_right = 0;
	}else
		if(struct_distances.right < D_LOW_LIMIT)
			danger_right = 1;

	printf("danger_front->%d\ndanger_left->%d\ndanger_right->%d\n\n", danger_front, danger_left, danger_right);
}

void calculateAjustment(){
	//cALCULA O AJUSTAMNETO NECESSÁRIO EM CADA UMA DAS DIREÇOES
	//VALOR CALCULADO É O VALOR A ENVIAR PARA O DRONE
	if(danger_front == 1){
		ajustment_front = -((struct_distances.front * slop) + 1);
	}
	if(danger_left == 1){
		ajustment_left = (struct_distances.left * slop) + 1;
	}
	if(danger_left == 1){
		ajustment_right = -((struct_distances.right * slop) + 1);
	}

	printf("NS_ajustment->%f\nEO_ajustement->%f\n\n", ajustment_front, ajustment_right + ajustment_left);
}

void calculateTaskTimes(){
	//MEDE O TEMPO QUE A TAREFA DEMORA A SER CONCLUIDA
	static int flag = 0;
	static double start, finish;
	struct timespec tstart={0,0}, tend={0,0};

	if(flag == 0){
    	clock_gettime(CLOCK_MONOTONIC, &tstart);
    	start = ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
    	flag = 1;
	}else{
	    clock_gettime(CLOCK_MONOTONIC, &tend);
	    finish = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec);

	    printf("Task took about %.9f seconds\n", finish - start);
	    flag = 0;
	}
}

void timeBetweenTaskCalls(){
	//MEDE O TEMPO ENTRE CHAMADAS Á TAREFA
	static int flag = 0;
	static double start, finish;
	struct timespec tstart={0,0}, tend={0,0};

	if(flag == 0){
    	clock_gettime(CLOCK_MONOTONIC, &tstart);
    	start = ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
    	flag = 1;
	}else{
	    clock_gettime(CLOCK_MONOTONIC, &tend);
	    finish = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec);

	    printf("time between task calls %.9f seconds\n\n", finish - start);

	    clock_gettime(CLOCK_MONOTONIC, &tstart);
    	start = ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
	}	
}