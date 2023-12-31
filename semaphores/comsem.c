// a tad more complex use of semaphore

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 1000LLU
static volatile int64_t global[NTHREADS]  = { 0 };
static pthread_t        threads[NTHREADS] = { 0 };
static volatile sem_t   semaphore         = { 0 }; // initialize with 1

// let's implement a kernel that executes a time consuming task, instead of a simple increment
static void* __attribute((noinline)) Kernel(void* input) {
    const int64_t thread_id = *((int64_t*) (input));
    sem_wait(&semaphore);
    for (size_t i = 0; i < thread_id; ++i) global[thread_id]++;
    sem_post(&semaphore);
    return NULL;
}

int main(void) {
    for (size_t i = 0; i < NTHREADS; ++i) threads[i] = i;
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, Kernel, global + i);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);

    int64_t sum = 0;
    for (size_t i = 0; i < NTHREADS; ++i) sum += global[i];
    wprintf_s("Sum is %lld\n", sum);
    return EXIT_SUCCESS;
}