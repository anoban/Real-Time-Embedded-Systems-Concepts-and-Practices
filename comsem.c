// a tad more complex use of semaphore

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NTHREADS 1000LLU
static volatile int64_t global[NTHREADS] = {0};
static pthread_t threads[NTHREADS] = {0};
static volatile sem_t semaphore= {0};

// let's implement a kernel that executes a time consuming task, instead of a simple increment
