#include "utility.h"
#include "ts_scheduler.h"
#include <stdio.h>


void __increment_time(void) { ++time_counter; };
void __reset_time(void) { time_counter = 0; };


void ts_barrier_init(TS_BARRIER *barrier, int threshold)
{
	barrier->threshold = threshold;
	barrier->cond = malloc(sizeof(*barrier->cond));
	barrier->mutex = malloc(sizeof(*barrier->mutex));
	pthread_cond_init(barrier->cond, NULL);
	pthread_mutex_init(barrier->mutex, NULL);
}


void ts_barrier_wait(TS_BARRIER *barrier)
{
    static int cnt;
    
    pthread_mutex_lock(barrier->mutex);
    ++cnt;
    if (cnt == barrier->threshold) {
        cnt = 0;
        pthread_cond_broadcast(barrier->cond);
    }
    else {
        pthread_cond_wait(barrier->cond, barrier->mutex);
    }
    pthread_mutex_unlock(barrier->mutex);
}


void ts_barrier_destroy(TS_BARRIER *barrier)
{
    pthread_cond_destroy(barrier->cond);
	pthread_mutex_destroy(barrier->mutex);
	free(barrier->cond);
	free(barrier->mutex);
	free(barrier);
}


TS_THREAD *ts_get_thread(void) 
{
    int curr_max_priority = TS_MAX_PRIO;

    while(cq_is_empty(&ready_threads[curr_max_priority])) {
        if (--curr_max_priority < 0)
            return NULL;
    }

    return (TS_THREAD*)cq_pop(&ready_threads[curr_max_priority]);
 
}


void *start_thread(void *params)
{
    TS_TH_PARAMS *aux = (TS_TH_PARAMS*)params;
    so_handler *func = aux->func;
    unsigned int priority = aux->priority;
    pthread_cond_t *cond = aux->cond;
    TS_BARRIER *barr = aux->barr;

    pthread_mutex_lock(&mutex);
    ts_barrier_wait(barr);
    pthread_cond_wait(cond, &mutex);
    pthread_mutex_unlock(&mutex);

    func(priority);

    running = ts_get_thread();
    __reset_time();

    if(running != NULL) 
        pthread_cond_signal(running->cond);
    
    return NULL;
}


void ts_preempt(void)
{
	TS_THREAD *next_thread;
    TS_THREAD *current_thread;

    if (time_counter == time_quantum) {
        current_thread = running;
        next_thread = ts_get_thread();

        if (next_thread != NULL && next_thread->priority >= current_thread->priority) {
            running = next_thread;
            __reset_time();

            cq_push(&ready_threads[current_thread->priority], current_thread);

            pthread_mutex_lock(&mutex);
            pthread_cond_signal(running->cond);
            pthread_cond_wait(current_thread->cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
        else {
            __reset_time();
            if (next_thread != NULL) {
                cq_push_front(&ready_threads[next_thread->priority], next_thread);
            }
        }
    }
}