#ifndef MY_BARRIER_H
#define MY_BARRIER_H

#include <pthread.h>

typedef struct {
    int count;
    int waiting;
    int generation;                
    pthread_mutex_t lock;
    pthread_cond_t cond;
} my_barrier_t;

void my_barrier_init(my_barrier_t *b, int count);
void my_barrier_destroy(my_barrier_t *b);
void my_barrier_wait(my_barrier_t *b);

#endif
