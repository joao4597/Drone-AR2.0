#include <math.h>
#include <stdio.h>
#include <stdlib.h> /* calloc */
#include <string.h>

#include <unistd.h>


#include "drk/video.h"
#include "drk/image_process.h"
#include "drk/utils.h"

#include "cmd.h"

#include <arpa/inet.h>
#include <sys/socket.h>



//extern int val[2];





int red_diff( uint8_t * in_uyvy , unsigned long len,uint8_t *p_red  ) 
{

	unsigned long  j = 0 ;
	//double aux;
//	uint8_t * p_red = (uint8_t*)calloc( 1, len+1 ) ;	
	for ( unsigned long  i = 2 ; i < len ; i = i + 4 )
	{
		if ( NULL==memcpy( p_red+j , in_uyvy+i ,1 ) ) return -1 ;
		j++;
	}

	
	#if 1 /* save gray format */
	char filename[30];
	sprintf(filename, "image_%lupx.craw", j ) ;
	FILE * fd = fopen(filename,"wb");
	int ret = (int)fwrite(p_red, j ,1,fd );
	//if (ret != 1 )
	//	PRINTF_FL_ERR("Failed to write file\n");
	fclose( fd ) ; 
	#endif
	
	//free(p_red);
	return j ;
}


int print_full( uint8_t * in_uyvy , unsigned long len  ) 
{

	#if 1 /* save gray format */
	char filename[30];
	sprintf(filename, "image_%lupx.raw", len ) ;
	FILE * fd = fopen(filename,"wb");
	int ret = (int)fwrite(in_uyvy, len ,1,fd );
//	if (ret != 1 )
//		PRINTF_FL_ERR("Failed to write file\n");
	fclose( fd ) ; 
	#endif

	return ret ;
}

uint8_t get_max(uint8_t* vec,int len)
{
	uint8_t max=0;
	int i;

	for(i=0;i<len;i++)
		if(vec[i]>max)
			max=vec[i];

return max;
}
void apply_threashold(uint8_t *vec,int len,float vale)
{
	int i;
	//printf("Thr: %lf\nlen: %d",vale,len);
	for(i=0;i<len;i++)
	{
		if(vec[i]>=vale)
			vec[i]=1;
		else
			vec[i]=0;
	}
}

void get_mass_center(uint8_t (*vec)[720][640],int *size,int *center)
{
	int accx=0;
	int accy=0;
	int count=0;
	int i,j;
	for(i=0;i<size[0];i++)
		for(j=0;j<size[1];j++)
		{
			accx+=j*(*vec)[i][j];
			accy+=i*(*vec)[i][j];
			if((*vec)[i][j]==1)
				count++;
		}
center[0]=accx/count;
center[1]=accy/count;


}

void vec_to_mat(uint8_t *vec,uint8_t (*mat)[720][640])
{
	int i,j,k=0;
	for(i=0;i<720;i++)
		for(j=0;j<640;j++)
		{
			(*mat)[i][j]=vec[k];
			k++;
		}

}


void get_relative_pos(int *size,int *center,pthread_mutex_t *lock)
{
	//float valx,valy;
	pthread_mutex_lock(lock);
	val[0]=size[1]/2-center[0];
	val[1]=size[0]/2-center[1];
	pthread_mutex_unlock(lock);
/*	if(val[0]>50)
		printf("Objecto a esquerda\n");
	else if(val[0]<-50)
		printf("Objecto a direita\n");
	else
		printf("Objecto centrado na horizontal\n");
	if(val[1]>50)
		printf("Objecto em cima\n");
	else if(val[1]<-50)
		printf("Objecto em baixo\n");
	else
		printf("Objecto centrado na vertical\n");

	printf("Desvio no x: %f\n",val[0]);
	printf("Desvio no y: %f\n",val[1]);*/
	center[0]=(int)val[0]/1;
	center[1]=(int)val[1]/1;
}	




void *image_drone_func(void *arg)
{ 
	th_arg *targ=arg;
	int ret ; 
	int size[2]={720,640};
	int center[2];
	uint8_t max;
	uint8_t mat[720][640];
	uint8_t *p_red=calloc( 1, (720*640)  ) ;
	//int i,j;
	char buf[30];
	#define DFLT_HEIGHT 0
	#define DFLT_WIDTH 0
	printf("abrir camera\n");
	if ( ( ret = drk_video_front_init( DFLT_HEIGHT , DFLT_WIDTH ) ) < 0  )
	{
		//PRINTF_FL_ERR( TABS "Failed to open front camera!\n" ) ;
		printf("Failed to open front camera!\n" ) ;
		//return ret ; 
	}
	
	
	
	/* Grab a frame */
	void * p_full_frame = calloc( 1, FRAME_SIZE_FRONT + 100  ) ; // confirm we need +100 
/*	if( p_full_frame == NULL )
	{
//		PRINTF_FL_ERR(TABS "Could not get mem.\n" ) ;
		return E_NO_MEM ;
	}*/
	
	
	/* gray image (just Y=luminance out of UYVY)  */
//	uint8_t * p_grayscale = (uint8_t*)calloc( 1, IMAGE_SIZE_FRONT ) ;
/*	if( p_grayscale == NULL )
	{
//		PRINTF_FL_ERR(TABS "Could not get mem.\n" ) ;
		return E_NO_MEM ;
	}*/
	
	/* SUBGRAY - subsampled gray image : */
	//const uint8_t subgray_smple_factor = 4 ; /* subsample factor on each dimension */
	//uint32_t subgray_width = VIDEO_WIDTH_FRONT / subgray_smple_factor ;
	//uint32_t subgray_height = VIDEO_HEIGHT_FRONT / subgray_smple_factor ;

	//uint32_t subgray_size = 
	//	subgray_width * subgray_height ; /* subgray size */
	//uint8_t * p_subgray = (uint8_t*)calloc( 1, subgray_size ) ;
/*	if( p_subgray == NULL )
	{
//		PRINTF_FL_ERR(TABS "Could not get mem.\n" ) ;
		return E_NO_MEM ;
	}*/


	
while(1)
{	
	uint32_t fsize = 0 ;
	if ( grab_frame( p_full_frame , &fsize ) < 0  || fsize != FRAME_SIZE_FRONT )
	{
		printf("Failed to grab a valid frame.\n" ) ;
		//return -1 ;
	}


	
	
	/* extract BW image */
	//UYVY_2_Y( p_full_frame , FRAME_SIZE_FRONT,  p_grayscale ) ;
	//PRINTF_FL(TABS "Converted frame to Grayscale\n");
	

	ret=red_diff( p_full_frame , FRAME_SIZE_FRONT ,p_red ) ;
//	PRINTF_FL(TABS "Got red red_diff, ret: %d\n",ret);
	max=get_max(p_red,ret);
//	printf("Max: %d\n",max);
	apply_threashold(p_red,ret,(max-10));//coef obtido experimentalmente, alterar para algo mais robusto

	//printf("Val: %u\n",p_red[2]);

	vec_to_mat(p_red,&mat);
	/*for(i=0;i<720;i++)
	{	
		printf("\n");
		for(j=0;j<640;j++)
			printf("%u ",mat[i][j]);
	}*/
	get_mass_center(&mat,size,center);
	get_relative_pos(size,center,targ->lock_pvt);
	
	
	
/*
	snprintf(buf, 30, "%d;%d\n",center[0],center[1]);

	if (sendto(targ->shared.sock, buf,strlen(buf), 0, (struct sockaddr*) &(targ->si_other), sizeof(targ->si_other)) == -1)
    {
       die("sendto()");
    }
 */
	//usleep(90000);
   // sleep(1);
}


}
