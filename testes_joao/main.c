#include <stdio.h>
#include <stdio.h>
#include <pthread.h>
#include "obstacle_avoidence.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <assert.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
//#include "image_drone.h"


int main(){

	pthread_t thread_obstacle;

    pthread_attr_t act_attr;
    pthread_attr_init(&act_attr);
    size_t act_desired_stack_size = 4000000;
    pthread_attr_setstacksize(&act_attr, act_desired_stack_size);
    

    if (pthread_create(&thread_obstacle, &act_attr, obstacle_avoid ,NULL) != 0){
        printf( "spawning thread: - Error - %d\n", strerror(errno));
        return -1 ;
    }

    pthread_join(thread_obstacle, NULL);

return 0;

}
