
#include <linux/joystick.h>
#include <errno.h>
#include <string.h>

#include <linux/types.h>
#include <linux/input.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#include <stdio.h> //printf
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#define SERVER "192.168.1.1"
#define BUFLEN 512  //Max length of buffer
#define PORT1 5556   //The port on which to send data
#define MAX 32765
#define PORT2 9015
#define PORT3 9016

extern int search;
extern pthread_mutex_t lock;

//typedef struct {
 //	        unsigned int time;      /* event timestamp in milliseconds */
// 	        short value;   /* value */
// 	        unsigned char type;     /* event type */
// 	        unsigned char number;   /* axis/button number */
//} js_ev;

typedef struct  {
    float x, y;
}axis;

//extern struct sockaddr_in si_other;

void die(char *s)
{
    perror(s);
    exit(1);
}


int abre(int *fd)
{
	(*fd)=open("/dev/input/js0",O_RDONLY | O_NONBLOCK);
	if(*fd>0)
	{
		printf("Ficheiro aberto\n");
		return 1;
	}
	else
	{	
		printf("f: %d\n",*fd);
		fprintf(stderr, "Value of errno: %d\n", errno);
      	perror("Error printed by perror");
      	return 0;
	}
}


int update_axis(axis a[2],struct js_event *ev)
{
	int indice;

	indice=(ev->number)/2;
	if(indice<2)
	{
		if(ev->number % 2 == 0)
	   	 	a[indice].x = 1.0*(ev->value)/(MAX*2.1);
	   	 	//a[indice].x = -0.8;
	   	 	
	    else
	    {
			a[indice].y = 1.0*(ev->value)/(MAX*0.1);
	    	if(indice==1)
	    		a[indice].y=-1*a[indice].y;
	    }
		
		return 1;
	}
	else
		return 0;
}

int open_connection(int *sock,struct sockaddr_in *si_other,uint32_t port)
{

//int slen=sizeof(si_other);
    if ( (*sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }
 
    memset((char *) si_other, 0, sizeof(si_other));
    si_other->sin_family = AF_INET;
    si_other->sin_port = htons(port);
  //  si_other.sin_addr = AF_INET;
     
    if (inet_aton(SERVER , &(si_other->sin_addr)) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        
        return 0;
    }


    printf("Socket aberta \n");
    return 1;
}




int send_cmd(int type,int nr,axis analog[2],int sock,int unsigned *seq,struct sockaddr_in si_other,double *v)
{
	assert(sizeof(int)==sizeof(float));
	char buff[1024];
	if(type==0)
	{	//printf("ola\n");
		//snprintf(buff, 1024, "AT*CONFIG=1,\"control:altitude_max\",\"10000\"");
		 snprintf(buff, 1024, "AT*FTRIM=\r");
		//snprintf(buff, 1024, "Ola\n");
	}
	else if(type==1)
	{
		switch (nr)
		{
			case 14: snprintf(buff, 1024, "AT*REF=,290718208\r" );
			break;
			case 13: snprintf(buff, 1024, "AT*REF=,290717696\r" );
			break;
		
			
		}
		if(nr==10)
		{
			pthread_mutex_lock(&lock);
			//printf("Desviox: %lf\nDesvioy: %lf\n",v[0],v[1]);
			//printf("Aquiii*****************************************\n");
			//snprintf(buff,1024,"AT*PCMD=,1,%d,%d,%d,%d\r",*(int*)(&(analog[0].x)),*(int*)(&(analog[0].y)),*(int*)(&(v[1])),*(int*)(&(v[0])));
			snprintf(buff,1024,"ET*PCMD=,1,%d,%d,\r",*(int*)(&(analog[0].x)),*(int*)(&(analog[0].y)));
			pthread_mutex_unlock(&lock);
		}
		else if(nr==15 && search==1)
		{
			snprintf(buff,1024,"ET*PCMD=,1,%d,%d,\r",*(int*)(&(analog[0].x)),*(int*)(&(analog[0].y)));
		}
	}
	else if(type==2)
	{
			snprintf(buff,1024,"AT*PCMD=,1,%d,%d,%d,%d\r",*(int*)(&(analog[0].x)),*(int*)(&(analog[0].y)),*(int*)(&(analog[1].y)),*(int*)(&(analog[1].x)));
	/*	else 
		{
			pthread_mutex_lock(&lock);
			//printf("Desviox: %lf\nDesvioy: %lf\n",v[0],v[1]);
			printf("Aquiii*****************************************\n");
			//snprintf(buff,1024,"AT*PCMD=,1,%d,%d,%d,%d\r",*(int*)(&(analog[0].x)),*(int*)(&(analog[0].y)),*(int*)(&(v[1])),*(int*)(&(v[0])));
			snprintf(buff,1024,"ET*PCMD=,1,%d,%d,\r",*(int*)(&(analog[0].x)),*(int*)(&(analog[0].y)));
			pthread_mutex_unlock(&lock);
		}*/
		
	}
	else if(type==3)
	{
		snprintf(buff, 1024, "AT*CONFIG=,\"control:altitude_max\",\"10000\"");
	} 
	else 
		return 1;
	(*seq) ++;
//	printf("ola2\n");
	printf("Envio:\t%s\n",buff);

	if (sendto(sock,buff, strlen(buff) , 0 , (struct sockaddr *) &si_other, sizeof(si_other))==-1){
        die("sendto()");
    }
    else
    	printf("enviado\n");

if(type==1 && nr == 12)
	return 0;
else 
	return 1;

}