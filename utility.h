#ifndef UM_HEADER
#define UM_HEADER

#include "circular_queue.h"
#include "so_scheduler.h"

#define TS_INIT_ERROR 		 -10
#define TS_INIT_SUCCESS		   0
#define TS_BAD_IO_DEVICE 	  -1
#define BARRIER_WAIT_THREADS   2
#define TS_IO_WAIT_SUCCESS     0

typedef struct {
	unsigned int threshold;
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
} TS_BARRIER;


typedef struct {
	unsigned int priority;
	so_handler *func;
	pthread_cond_t *cond;
	TS_BARRIER *barr;
} TS_TH_PARAMS;


typedef struct {
	int priority;
	pthread_cond_t *cond;
	pthread_t thread;
} TS_THREAD;


/* GLOBALS USED BY THREAD SCHEDULER */
int time_quantum;
int time_counter;
CIRCULAR_QUEUE threads;
CIRCULAR_QUEUE ready_threads[TS_MAX_PRIO + 1];
CIRCULAR_QUEUE waiting_threads[TS_MAX_NUM_EVENTS];
TS_THREAD *running;
pthread_mutex_t mutex;
int io_events;


void __increment_time(void);
void __reset_time(void);

void ts_preempt(void);
void *start_thread(void*);
TS_THREAD *ts_get_thread(void);

void ts_barrier_init(TS_BARRIER*, int);
void ts_barrier_destroy(TS_BARRIER*);
void ts_barrier_wait(TS_BARRIER*);



#endif