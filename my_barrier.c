#include "my_barrier.h"
#include <pthread.h>
#include <stdlib.h>

void my_barrier_init(my_barrier_t *b, int count) {
    b->count = count;
    b->waiting = 0;
    b->generation = 0;          
    pthread_mutex_init(&b->lock, NULL);
    pthread_cond_init(&b->cond, NULL);
}

void my_barrier_destroy(my_barrier_t *b) {
    pthread_mutex_destroy(&b->lock);
    pthread_cond_destroy(&b->cond);
}

void my_barrier_wait(my_barrier_t *b) {
    pthread_mutex_lock(&b->lock);

    int current_generation = b->generation;
    b->waiting++;

    if (b->waiting == b->count) {
        b->waiting = 0;
        b->generation++;                    
        pthread_cond_broadcast(&b->cond);    
    } else {
        while (current_generation == b->generation) {
            pthread_cond_wait(&b->cond, &b->lock); 
        }
    }

    pthread_mutex_unlock(&b->lock);
}
