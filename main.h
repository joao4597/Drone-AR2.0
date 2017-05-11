//#include <pthread.h>

typedef struct {
pthread_mutex_t *lock;
int sock;
int *seq;
}drone_com;