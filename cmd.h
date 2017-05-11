
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

#include "main.h" 

#define PORT2 9015
#define PORT1 5556 
#define MAX_SIZE 1024 
#define SERVER "192.168.1.1"


void die(char *s)
{
    perror(s);
    exit(1);
}

typedef struct {
	struct sockaddr_in si_other;
	int sock;
	drone_com shared;
}th_arg;


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
	
	//int *dr_sock=arg;
	struct sockaddr_in si_other1;
	th_arg *targ=arg;
	struct sockaddr_in si_me;
	int slen=sizeof(si_other1);
	int recv_len;
	int sock;
	int ret;
	char buf[MAX_SIZE];
	char buffer[MAX_SIZE];
	char *buf_send1;
	//int seq=0;

	//int estado=0;

	ret=open_server_socket(&sock,&si_other1,&si_me,PORT2);
	//int slen=sizeof(si_me);
	while(1)
	{
		memset(buf,0,sizeof(buf));
		memset(buffer,0,sizeof(buffer));
		if ((recv_len = recvfrom(sock,buf,MAX_SIZE,0,NULL,(socklen_t *) &slen)) == -1)
		{
			die("recvfrom()");
		}
		
		
		strtok_r (buf, "=", &buf_send1);
		snprintf(buffer, 1024, "%s=%u%s\r",buf,seq,buf_send1);
		pthread_mutex_lock(&(targ->shared.lock));
		(*(targ->shared.seq))++;
		pthread_mutex_unlock(&(targ->shard.lock));
		printf("Envio: %s\n",buffer);

	if (sendto(targ->shared.sock,buffer, strlen(buffer) , 0 , (struct sockaddr *) &(targ->si_other), sizeof(struct sockaddr_in))==-1){
        die("sendto()");
    }
    else
    	printf("enviado\n");

	}




}