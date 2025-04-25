#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <float.h>
#include "my_barrier.h"

#define REPEATS 1000
#define WORK_SIZE 1000

pthread_barrier_t pthread_barrier;
my_barrier_t my_barrier;

void do_work(int thread_id) {
    float sum = 0.0;
    for (int i = 0; i < WORK_SIZE; i++) {
        sum += (float)i;
    }
}

void *pthread_barrier_thread_func(void *arg) {
    int thread_id = *(int *)arg;
    for (int i = 0; i < REPEATS; i++) {
        do_work(thread_id);
        pthread_barrier_wait(&pthread_barrier);
    }
    return NULL;
}

void *my_barrier_thread_func(void *arg) {
    int thread_id = *(int *)arg;
    for (int i = 0; i < REPEATS; i++) {
        do_work(thread_id);
        my_barrier_wait(&my_barrier);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "Error: num_threads must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    pthread_t threads[num_threads];
    int thread_ids[num_threads];
    struct timespec start, end;
    double elapsed_time;

    for (int i = 0; i < num_threads; i++) {
        thread_ids[i] = i;
    }

    printf("Testing with pthread_barrier_t...\n");
    pthread_barrier_init(&pthread_barrier, NULL, num_threads);
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, pthread_barrier_thread_func, &thread_ids[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time with pthread_barrier_t: %f seconds\n", elapsed_time);
    pthread_barrier_destroy(&pthread_barrier);

    printf("Testing with my_barrier_t...\n");
    my_barrier_init(&my_barrier, num_threads);
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, my_barrier_thread_func, &thread_ids[i]);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time with my_barrier_t: %f seconds\n", elapsed_time);

    my_barrier_destroy(&my_barrier);

    return 0;
}
