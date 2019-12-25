#include "so_scheduler.h"
#include "circular_queue.h"
#include "utility.h"


int ts_init(unsigned int quantum, unsigned int io)
{
    int i;
    if (quantum == 0 || time_quantum != 0 || io > TS_MAX_NUM_EVENTS) 
        return TS_INIT_ERROR;

    time_quantum = quantum;
    running = NULL;
    io_events = io;
    cq_init(&threads);
	for (i = 0; i < TS_MAX_PRIO + 1; ++i)
        cq_init(&ready_threads[i]);

    for (i = 0; i < TS_MAX_NUM_EVENTS; ++i)
        cq_init(&waiting_threads[i]);
    
    pthread_mutex_init(&mutex, NULL);

    return TS_INIT_SUCCESS;
}


void ts_end(void)
{
    TS_THREAD *th;

    if (! time_quantum)
        return;

    th = (TS_THREAD*)cq_pop(&threads);

    while (th != NULL) {
		pthread_join(th->thread, NULL);
		pthread_cond_destroy(th->cond);
        free(th->cond);
		free(th);
        th = (TS_THREAD*)cq_pop(&threads);
    }
    time_quantum = 0;
}


void ts_exec(void)
{
    __increment_time();
    ts_preempt();
}


pthread_t ts_fork(so_handler *func, unsigned int priority)
{
	pthread_t new_tid;
    TS_THREAD *current_thread;
    TS_THREAD *new_thread = malloc(sizeof(*new_thread));;
    if (new_thread == NULL)
        return INVALID_TID;

    __increment_time();

    TS_TH_PARAMS* th_params = malloc(sizeof(*th_params));
    if (th_params == NULL)
        return INVALID_TID;

    if (func == NULL || priority > TS_MAX_PRIO)
        return INVALID_TID;

    th_params->func = func;
    th_params->priority = priority;
	th_params->cond = malloc(sizeof(*th_params->cond));
	pthread_cond_init(th_params->cond, NULL);
	th_params->barr = malloc(sizeof(*th_params->barr));
    
    ts_barrier_init(th_params->barr, BARRIER_WAIT_THREADS);

    if (pthread_create(&new_tid, NULL, start_thread, th_params) < 0) {
        return INVALID_TID;
    }

	new_thread->thread = new_tid;
	new_thread->priority = priority;
	new_thread->cond = th_params->cond;

    cq_push(&threads, new_thread);
    ts_barrier_wait(th_params->barr);

    if (running == NULL) {
        running = new_thread;
        pthread_mutex_lock(&mutex);
		pthread_cond_signal(new_thread->cond);
		__reset_time();
		pthread_mutex_unlock(&mutex);
    }
    else if (running->priority < priority) {
        current_thread = running;
        cq_push(&ready_threads[current_thread->priority], current_thread);
        running = new_thread;
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(new_thread->cond);
        __reset_time();
        pthread_cond_wait(current_thread->cond, &mutex);
        pthread_mutex_unlock(&mutex);           
    }
    else {
        pthread_mutex_lock(&mutex);
        cq_push(&ready_threads[priority], new_thread);
        pthread_mutex_unlock(&mutex);
        ts_preempt();
    }

    ts_barrier_destroy(th_params->barr);
	free(th_params); 

	return new_tid;
}


int so_signal(unsigned int io)
{
    TS_THREAD *current_thread;
    TS_THREAD *temp;
    int cnt = 0;

    if (io >= io_events || io < 0)
        return TS_BAD_IO_DEVICE;

    __increment_time();

    current_thread = running;
    temp = (TS_THREAD*)cq_pop(&waiting_threads[io]); 
    while (temp != NULL) {
        ++cnt;
        if (temp->priority > running->priority) {
            /* Il pun pe actual */
            cq_push(&ready_threads[running->priority], running);
            running = temp;
            __reset_time();
        }
        else {
            cq_push(&ready_threads[temp->priority], temp);
        }

        temp = (TS_THREAD*)cq_pop(&waiting_threads[io]); 
    }

    if (current_thread == NULL)
        return TS_BAD_IO_DEVICE;

    if (current_thread == running) {
        ts_preempt();
    }
    else {
        pthread_mutex_lock(&mutex);
        pthread_cond_signal(running->cond);
        pthread_cond_wait(current_thread->cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }

    return cnt;
}


int ts_wait(unsigned int io)
{
    TS_THREAD *current_thread;
    if (io >= io_events || io < 0)
        return TS_BAD_IO_DEVICE;
    
    __increment_time();

	pthread_mutex_lock(&mutex);

    current_thread = running;
    cq_push(&waiting_threads[io], current_thread);

    running = ts_get_thread();
    if (running != NULL) {
        pthread_cond_signal(running->cond);
    }

    __reset_time();
    pthread_cond_wait(current_thread->cond, &mutex);
    pthread_mutex_unlock(&mutex);           

    
    return TS_IO_WAIT_SUCCESS;
}
