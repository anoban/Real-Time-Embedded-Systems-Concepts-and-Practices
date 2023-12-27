// code example for increment-decrement

#include <pthread.h>
#include <sched.h>      // header for schedulers
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>     // EXIT_SUCCESS

#define NTHREADS   6LLU // as long as the thread count is an even number, sum will be 0 ultimately.
#define UPPERBOUND 100LLU
// as we increase these numbers, the chances for data races increase and the likelihood of getting a non-zero sum increases.

static volatile pthread_t threads[NTHREADS];
// we really do not need to use a struct to thread indices
static volatile uint64_t  threadIndices[NTHREADS];
static volatile uint64_t  sum = 0; // hotspot for data races

static void* __attribute((noinline)) TaskInc(void* thread_props) {
    const uint64_t* const restrict threadId = (uint64_t*) thread_props;
    for (uint64_t i = 0; i < UPPERBOUND; ++i) {
        sum += i;
        printf("Increment Worker Id: %4lu, sum: %10lu\n", *threadId, sum);
    }
    return NULL;
}

static void* __attribute((noinline)) TaskDec(void* thread_props) {
    const uint64_t* const restrict threadId = (uint64_t*) thread_props;
    for (uint64_t i = 0; i < UPPERBOUND; ++i) {
        sum -= i;
        printf("Decrement Worker Id: %4lu, sum: %10lu\n", *threadId, sum);
    }
    return NULL;
}

// Linux typically load balances around every 200 milliseconds.

int main(void) {
    for (size_t i = 0; i < NTHREADS; ++i) threadIndices[i] = i;

    // make threads with even ids do incrementation
    for (size_t i = 0; i < NTHREADS; i += 2) pthread_create(threads + i, NULL, TaskInc, (void*) (threadIndices + i));
    // make threads with even ids do decrementation
    for (size_t i = 1; i < NTHREADS; i += 2) pthread_create(threads + i, NULL, TaskDec, (void*) (threadIndices + i));
    for (size_t i = 1; i < NTHREADS; ++i) pthread_join(threads[i], NULL);

    printf("Finally sum is %lu\n", sum);
    puts("Completed!");
    return EXIT_SUCCESS;
}