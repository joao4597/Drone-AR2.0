#define D_HIGH_LIMIT 1.30
#define D_LOW_LIMIT 1.0

#define SPEED_AT_DIS_LOW 0.6
#define SPEED_AT_DIS_HIGH 0.1
#define B 1

//#include "cmd.h"


void *obstacle_avoid(void *atr);
void avoidObstacleHandler(int sig);


typedef struct sensores_ajustment{
	int NS_ajustment;
	int EW_ajustment;
	int NS;
	int EW;
}ajustments;

extern ajustments struct_ajustments;
extern int hoover;
extern int obstacle_flag;