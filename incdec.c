// code example for increment-decrement

#include <pthread.h>
#include <sched.h>  // header for schedulers
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS

#define NTHREADS   40LLU
#define UPPERBOUND 10LLU

static volatile pthred_t threads[NTHREADS];
// we really do not need to use a struct to thread indices
static volatile uint64_t threadIndices[NTHREADS];
static volatile uint64_t sum = 0; // hotspot for data races

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
    puts("Completed!");
    return EXIT_SUCCESS;
}