#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include "control.h"
#include <string.h>
#include <pthread.h>



int abre(int *fd);
int flag;
int update_axis(axis a[2],struct js_event *ev);
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
int send_cmd(int type,int nr,axis analog[2],int sock,int unsigned *seq,struct sockaddr_in si_other,double *v);
int open_connection(int *sock,struct sockaddr_in *si_other,uint32_t port);
int search;
void *video_thread(void *arg)
{
double *vec=arg;
struct sockaddr_in si_other;
int slen=sizeof(si_other);
int recv_len;
int sock;
int ret;
char buf[30];
char desvio_x[10];
char desvio_y[10];
int i,j;
unsigned int dum;
int estado=0;
j=0;
ret=open_connection(&sock,&si_other,PORT2);

ret=send_cmd(0,0,0,sock,&dum,si_other,NULL);

	while(1)
	{
		if ((recv_len = recvfrom(sock, buf, 30, 0, NULL,(socklen_t *) &slen)) == -1)
		{
			die("recvfrom()");
		}
		j=0;
		estado=0;
		for(i=0;i<recv_len;i++)
		{
			if(estado==0)
			{
				if(buf[i]==';')
				{

					//memcpy(desvio_x,&buf[0],(i+1)*sizeof(char));
					desvio_x[i]='\0';
					estado=1;
					//memcpy(desvio_y,&buf[i+1],recv_len-3);
					//j=i;
				}
				else
					desvio_x[i]=buf[i];
			}
			else if(estado==1)
			{


				if(buf[i]=='\n')
				{
					//memcpy(desvio_y,&buf[j+1],(i-j-2)*sizeof(char));
					desvio_y[j]='\0';
					i=recv_len+1;
				}
				else
				{
					desvio_y[j]=buf[i];
					j++;
				}

			}
		}
	//	printf("Recebido : %s\n",buf);
	//	printf("Desvio_x: %s\n",desvio_x);
	//	printf("Desvio_y: %s\n",desvio_y);
		pthread_mutex_lock(&lock);
		vec[0]=-1*atoi(desvio_x)/370;
		vec[1]=atoi(desvio_y)/370;
	//	printf("Desvio_x: %lf\n",vec[0]);
	//	printf("Desvio_y: %lf\n",vec[1]);
	//	printf("Desvio_x doule: %lf\n",vec[0]/370);
	//	printf("Desvio_y double: %lf\n",vec[1]/370);
		if(flag!=0)
		{
			pthread_mutex_unlock(&lock);
			return NULL;
		}
		pthread_mutex_unlock(&lock);
	}
}


int main(int argc, char **argv)
{
setbuf(stdout, NULL);
int fd;
flag=0;
double vec[2];
search =0;
pthread_t vid;
//pthread_create(&vid,NULL,video_thread,&vec[0]);
int ret;
int sock;
char buf[30];
//int max=0;
int unsigned seq;
struct sockaddr_in si_other;
//int i;
int slen=sizeof(si_other);
int recv_len;
//int errnum;
axis analog[2];
analog[0].x=0;
analog[0].y=0;
analog[1].x=0;
analog[1].y=0;
struct js_event ev;



ret=abre(&fd);
seq=1;
if(ret==0)
return 0;



ret=open_connection(&sock,&si_other,PORT2);


if(ret==0)
return 0;


//ret=send_cmd(0,ev.number,analog,sock,&seq,si_other);

	while(1)
	{
		ret=read(fd,&(ev),sizeof(struct js_event));
		if(ret==0)
			break;
		if(ev.type==JS_EVENT_BUTTON)
		{
			//printf("Number: %8d\nVal:%d\n",ev.number,ev.value);
			if(ev.number==10)
				search=ev.value;
			else
			ret=send_cmd(1,ev.number,analog,sock,&seq,si_other,NULL);
			if(ret==0)
			{	
				pthread_mutex_lock(&lock);
				flag=1;
				pthread_mutex_unlock(&lock);
				break;
			}
		}
		else if(ev.type==JS_EVENT_AXIS && (ev.number<4))
		{
			update_axis(analog,&ev);
			send_cmd(2,ev.number,analog,sock,&seq,si_other,&vec[0]);
		//	for(i=0;i<2;i++)
			//	printf("X[%d]: %f, Y[%d]: %f\n",i,analog[i].x,i, analog[i].y);
			
		}

	//	printf("yup\n");
    	
	}
	
	close(fd);
//	close(sock);

return 0;

}