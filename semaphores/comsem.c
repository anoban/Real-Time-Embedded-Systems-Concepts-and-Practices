// a tad more complex use of semaphore

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NTHREADS 256LLU
#define MAXITER  1024LLU
static volatile uint64_t global            = 0;
static pthread_t         threads[NTHREADS] = { 0 };
static sem_t             semaphore         = { 0 }; // initialize with 1

// let's implement a kernel that executes a time consuming task, instead of a simple increment
static void* __attribute((noinline)) Kernel(void* input) {
    sem_wait(&semaphore);
    for (size_t i = 0; i < MAXITER; ++i) global++;
    // or global += NTHREADS; but NO
    sem_post(&semaphore);
    return NULL;
}

// will be more time consuming
static void* __attribute((noinline)) _Kernel(void* input) {
    for (size_t i = 0; i < MAXITER; ++i) {
        sem_wait(&semaphore);
        global++;
        sem_post(&semaphore);
    }

    return NULL;
}

int main(void) {
    clock_t start = 0, elapsed = 0;
    for (size_t i = 0; i < NTHREADS; ++i) threads[i] = i;

    sem_init(&semaphore, 0, 1);
    start = clock();
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, Kernel, NULL);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    elapsed = clock() - start;
    sem_close(&semaphore);
    wprintf_s(L"Sum is %lld, took %ld ticks!\n", global, elapsed);

    start = elapsed = global = 0;

    sem_init(&semaphore, 0, 1);
    start = clock();
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, _Kernel, NULL);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    elapsed = clock() - start;
    sem_close(&semaphore);
    wprintf_s(L"Sum is %lld, took %ld ticks!\n", global, elapsed);

    return EXIT_SUCCESS;
}