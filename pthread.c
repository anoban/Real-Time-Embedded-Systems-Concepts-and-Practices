// code example demonstrating the use of POSIX threads
// compile with gcc pthread.c -Wall -Wpedantic -std=c17 -O3 -o pthread.exe -lpthread

// Linux by default is a symmetric multi-processor systems with load balancing

#include <pthread.h>
#include <sched.h>       // header for schedulers
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS 1024LLU // number of threads to execute in parallel

// THERE SEEMS TO BE A WIN32 IMPLEMENTATION OF ThreadParams_t in sourceannotations.h YIKES!

#pragma pack(push, 8)
typedef struct tag_ThreadParams {
        uint64_t threadIndex;
} ThreadParams_t;
#pragma pack(pop)

static pthread_t      threads[NTHREADS]; // pthread structs
static ThreadParams_t thread_parameters[NTHREADS];

// a pthread task needs to have void * (*)(void *) signature.
static inline void*   Task(void* thread_parameter) {
    // this explicit cast is used since the function entails type erasure.
    const ThreadParams_t* threadparam = (ThreadParams_t*) thread_parameter;
    uint64_t              sum         = 0;
    for (uint64_t i = 0; i < threadparam->threadIndex + 1; ++i) sum += i;
    // in POSIX systems long itself in 64 bits so no need for double 'l's in format specifiers.
    printf("pthread index: %4lu, sum(0..%04ld) = %10lu\n", threadparam->threadIndex, threadparam->threadIndex, sum);
    return NULL;
}

int main(void) {
    for (uint64_t i = 0; i < NTHREADS; ++i) {
        thread_parameters[i].threadIndex = i;
        pthread_create(
            &threads[i] /* pointer to pthread_t struct */,
            NULL /* thread attributes - NULL denotes defaults attributes, this is where RT attributes are speciifed if needed */,
            Task /* function pointer to be executed by the spawned thread */,
            (void*) (thread_parameters + i)                                 /* arguments to the function pointer */
        );
    }

    for (uint64_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL); // join the spawned threads with the main()'s thread
    puts("Completed!");
    return EXIT_SUCCESS;
}

// When we examine the results, they suggest that the default pthread implementation is a best effort SMP thread.
// They'll be treated fairly and assigned to the core that is lest busy, etc.
// This is the reason for the non-deterministic behaviour of the results
// i.e we cannot control which thread gets executed in which order. This happens outside of the programmers control
// Every time you rerun this programme, we'll get a different sequence of thread execution.
