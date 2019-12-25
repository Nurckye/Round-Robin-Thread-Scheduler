#include "circular_queue.h"

CQ_NODE *cq_create_node(void *new_value)
{
    CQ_NODE *temp = malloc(sizeof(*temp));
    temp->value = new_value;
    return temp;
}


void cq_init(CIRCULAR_QUEUE* cq_obj)
{
    cq_obj->size = 0;
    cq_obj->tail = cq_obj->head = NULL;
}


void cq_push(CIRCULAR_QUEUE *cq_obj, void *new_value)
{
    CQ_NODE *temp;
    if (cq_obj->head == NULL) {
        cq_obj->head = cq_create_node(new_value);
        cq_obj->tail = cq_obj->head;
        cq_obj->head->next = cq_obj->head;
    }    
    else {
        temp = cq_create_node(new_value);
        cq_obj->tail->next = temp;
        cq_obj->tail = temp;
        temp->next = cq_obj->head; 
    }
    cq_obj->size++;
}


int cq_is_empty(CIRCULAR_QUEUE *cq_obj)
{
    if (cq_obj == NULL)
        return 1;

    return cq_obj->size == 0;
}


void* cq_pop(CIRCULAR_QUEUE *cq_obj)
{
    CQ_NODE *temp;
    void *aux_val;
    if (cq_is_empty(cq_obj))
        return NULL;

    temp = cq_obj->head;
    aux_val = temp->value;
    if (cq_obj->head->next == cq_obj->head) {
        cq_obj->head = cq_obj->tail = NULL;
    }
    else {
        cq_obj->tail->next = cq_obj->head->next;
        cq_obj->head = cq_obj->head->next;
    }

    free(temp);
    cq_obj->size--;
    return aux_val;
}


void cq_push_front(CIRCULAR_QUEUE* cq_obj, void *new_value)
{
    CQ_NODE *temp;

    if (cq_obj->head == NULL) {
        cq_obj->head = cq_create_node(new_value);
        cq_obj->tail = cq_obj->head;
        cq_obj->head->next = cq_obj->head;
    }    
    else {
        temp = cq_create_node(new_value);
        temp->next = cq_obj->head;
        cq_obj->tail->next = temp;
        cq_obj->head = temp;
    }

    cq_obj->size++;
}
