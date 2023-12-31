// a tad more complex use of semaphore

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 1000LLU
static volatile uint64_t global            = 0;
static pthread_t         threads[NTHREADS] = { 0 };
static volatile sem_t    semaphore         = { 0 }; // initialize with 1

// let's implement a kernel that executes a time consuming task, instead of a simple increment
static void* __attribute((noinline)) Kernel(void* input) {
    sem_wait(&semaphore);
    for (size_t i = 0; i < NTHREADS; ++i) global++;
    // or global += NTHREADS; but NO
    sem_post(&semaphore);
    return NULL;
}

// will be more time consuming
static void* __attribute((noinline)) _Kernel(void* input) {
    for (size_t i = 0; i < NTHREADS; ++i) {
        sem_wait(&semaphore);
        global++;
        sem_post(&semaphore);
    }

    return NULL;
}

int main(void) {
    for (size_t i = 0; i < NTHREADS; ++i) threads[i] = i;

    sem_init(&semaphore, 0, 1);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, Kernel, NULL);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    sem_close(&semaphore);
    wprintf_s(L"Sum is %lld\n", global);

    global = 0;

    sem_init(&semaphore, 0, 1);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, _Kernel, NULL);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    sem_close(&semaphore);
    wprintf_s(L"Sum is %lld\n", global);

    return EXIT_SUCCESS;
}