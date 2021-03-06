
#include <string.h>
#include <errno.h>
#include <string.h>

#include <linux/types.h>
#include <linux/input.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

//#include "image_drone.h" 

#define PORT2 9015
#define PORT3 9016
#define PORT1 5556 
#define MAX_SIZE 1024 
#define SERVER "192.168.1.1"
#define BILLION 1E9

float val[2];
extern int cmd_flag;

extern char send_temp[MAX_SIZE];
extern char cmd_prefix[MAX_SIZE];

void die(char *s)
{
    perror(s);
    //exit(1);
}

typedef struct {
    pthread_mutex_t *lock;
    int sock;
    int *seq;
}drone_com;

typedef struct {
    struct sockaddr_in si_other;
    pthread_mutex_t *lock_pvt;
    drone_com shared;
}th_arg;





int open_connection(int *sock,struct sockaddr_in *si_other,uint32_t port)
{
//char send_temp[MAX_SIZE];
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





int open_server_socket(int *sock,struct sockaddr_in *si_other,struct sockaddr_in *si_me,uint32_t port)
{

   // struct sockaddr_in si_me, si_other;
    // int sock1;
   // int  slen = sizeof((*si_other)) , recv_len;
    //char buf[30];
   // struct sockaddr_in si_me1;
    printf("Vou abrir socket\n");
    if (((*sock)=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("sock");
        return 0;
    }
    printf("Abri socket\n");
    memset(si_me, 0, sizeof((*si_me)));
     
    si_me->sin_family = AF_INET;
    si_me->sin_port = htons(PORT2);
    si_me->sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    printf("Vou ligar a socket á porta\n");
    if( bind((*sock) , (struct sockaddr*)si_me, sizeof( struct sockaddr_in ) ) == -1)
    {
        die("bind");
        return 0;
    }
    return 1;
}






void *cmd_thread_func(void *arg)
{
	struct sockaddr_in si_other1;
	th_arg *targ=arg;
	struct sockaddr_in si_me;
	int slen=sizeof(si_other1);
	int recv_len;
	int sock;
    int i;
    int xd;
    int xy;
	int ret;
	char buf[MAX_SIZE];
	char buffer[MAX_SIZE];
    assert(sizeof(int)==sizeof(float));
    float x;
    int j;
    int size;
    float y;
	char *buf_send1;
    char buf_send2[MAX_SIZE];
	ret=open_server_socket(&sock,&si_other1,&si_me,PORT2);
    struct timespec requestStart, requestEnd;
    double time_lapsed;
    double max=0;
    
    

	while(1)
	{
        clock_gettime(CLOCK_REALTIME, &requestStart);
		memset(buf,0,sizeof(buf));
		memset(buffer,0,sizeof(buffer));
        x=-1.0*val[0]/800;
        y=val[1]/800;
        xd=*((int*)(&x));
        xy=*((int*)(&y));
		if ((recv_len = recvfrom(sock,buf,MAX_SIZE,0,NULL,(socklen_t *) &slen)) == -1)
		{
			die("recvfrom()");
		}
		memset(send_temp,0,MAX_SIZE);
        size=strlen(buf);
        for(i=0;i<size;i++)
        {
            if(buf[i]=='=')
            {
                memcpy(buffer,&buf[i+1],size-i-1);
                size=size-i-1;
                break;
            }
        }
        strtok_r (buf, "=", &buf_send1);
        if(buf[0]=='E')
        {
            pthread_mutex_lock(targ->lock_pvt);
            pthread_mutex_unlock(targ->lock_pvt);
            snprintf(buf_send2, 1024, "0,%d",xd);
            
            buf[0]='A';
            snprintf(buffer, 1024, "%s=%u%s\r",buf,*(targ->shared.seq),buf_send1);
            j=strlen(buffer)-2;

            memcpy(&buffer[j],buf_send2,sizeof(char)*strlen(buf_send2));
            buffer[j+strlen(buf_send2)]='\r';
        }
     

      
        pthread_mutex_lock(targ->shared.lock);
        cmd_flag=1;
        memset(cmd_prefix,0,MAX_SIZE);
        memset(send_temp,0,MAX_SIZE);
        memcpy(cmd_prefix,buf,strlen(buf));
        for(i=0;i<size;i++)
            send_temp[i]=buffer[i];
        pthread_mutex_unlock(targ->shared.lock);
        clock_gettime(CLOCK_REALTIME, &requestEnd);
        time_lapsed = ( requestEnd.tv_sec - requestStart.tv_sec )+( requestEnd.tv_nsec - requestStart.tv_nsec )/ BILLION;
        if(time_lapsed>max && time_lapsed<0.5)
            max=time_lapsed;

        //printf("Time lapsed: %lf\tMax: %lf\n",time_lapsed,max);
        }

}





















/* if (sendto(targ->shared.sock,buffer, strlen(buffer) , 0 , (struct sockaddr *) &(targ->si_other), sizeof(struct sockaddr_in))==-1){
        die("sendto()");
    }
    else
        printf("enviado\n");

    }
*/ 