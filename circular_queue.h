#ifndef CQ_HEADER
#define CQ_HEADER

#include <stdlib.h>
#include "ts_scheduler.h"


typedef struct circular_queue_node {
    struct circular_queue_node *next;
    void *value;
} CQ_NODE;


typedef struct {
    CQ_NODE *head;
    CQ_NODE *tail;
    int size;
} CIRCULAR_QUEUE; 


CQ_NODE *cq_create_node(void*);


void cq_push(CIRCULAR_QUEUE*, void*);
void cq_push_front(CIRCULAR_QUEUE*, void*);
void *cq_pop(CIRCULAR_QUEUE*);
int cq_is_empty(CIRCULAR_QUEUE*);
void cq_init(CIRCULAR_QUEUE*);


#endif