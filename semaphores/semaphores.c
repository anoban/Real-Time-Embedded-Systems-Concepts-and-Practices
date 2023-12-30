#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NTHREADS 10LLU
#define MAXITER  100LLU

static volatile size_t global_sum        = 0; // this is the global variable all threads will be trying to read from and write to.
static _Atomic size_t  atomic_sum        = 0; // protected type by virtue
static volatile size_t guarded_sum       = 0; // semaphore protected

static pthread_t       threads[NTHREADS] = { 0 };
static volatile sem_t  semaphore         = { 0 };

void* __attribute__((noinline)) IncKernel(void* param) { //  implementation vulnerable to data races
    for (size_t i = 0; i < MAXITER; ++i) global_sum++;   // increment the global sum
    return NULL;
}

void* __attribute__((noinline)) DecKernel(void* param) { //  implementation vulnerable to data races
    for (size_t i = 0; i < MAXITER; ++i) global_sum--;   //  decrement the global sum
    return NULL;
}

void* __attribute__((noinline)) AtomicIncKernel(void* param) {
    for (size_t i = 0; i < MAXITER; ++i) atomic_sum++;
    return NULL;
}

void* __attribute__((noinline)) AtomicDecKernel(void* param) {
    for (size_t i = 0; i < MAXITER; ++i) atomic_sum--;
    return NULL;
}

int main(void) {
    // unsafe implementation, prone to data races
    for (size_t i = 0; i < NTHREADS; i += 2)
        if (pthread_create(threads + i, NULL, IncKernel, NULL)) fputs("error in pthread_create", stderr);
    for (size_t i = 1; i < NTHREADS; i += 2)
        if (pthread_create(threads + i, NULL, DecKernel, NULL)) fputs("error in pthread_create", stderr);

    printf("After execution of %llu threads, global_sum is %zd\n", NTHREADS, global_sum);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    memset(threads, 0, sizeof(pthread_t) * NTHREADS);

    // same implementation, using _Atomic sum
    for (size_t i = 0; i < NTHREADS; i += 2)
        if (pthread_create(threads + i, NULL, AtomicIncKernel, NULL)) fputs("error in pthread_create", stderr);
    for (size_t i = 1; i < NTHREADS; i += 2)
        if (pthread_create(threads + i, NULL, AtomicDecKernel, NULL)) fputs("error in pthread_create", stderr);

    printf("After execution of %llu threads, atomic_sum is %zd\n", NTHREADS, atomic_sum);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    memset(threads, 0, sizeof(pthread_t) * NTHREADS);

    // semaphore guarded implementation

    // int sem_init(sem_t *sem, int pshared, unsigned int value);
    sem_init(&semaphore, 0, 0); // the second flag is used to signal whether this semaphore is used by multiple processes.
                                // if so, it must be 1 else it should be 0
                                // the third argument is the value the semaphore is initialized with
                                
    sem_destroy(&semaphore);

    return EXIT_SUCCESS;
}
