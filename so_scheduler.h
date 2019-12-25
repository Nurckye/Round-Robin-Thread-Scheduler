/* AUTHOR: Radu Nitescu 
   DATE: December 2019 */

#ifndef TS_SCHEDULER
#define TS_SCHEDULER

#include <pthread.h>

#define TS_MAX_PRIO 5
#define TS_MAX_NUM_EVENTS 256
#define INVALID_TID ((pthread_t)0)


typedef void (so_handler)(unsigned int);

int ts_init(unsigned int, unsigned int);
pthread_t ts_fork(so_handler*, unsigned int);
int ts_wait(unsigned int);
int so_signal(unsigned int);
void ts_exec(void);
void ts_end(void);

#endif 

