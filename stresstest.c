#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>

// disable optimizations
// gcc -Wall -Wpedantic -std=c17 -O0 -g0 stresstest.c -o stresstest.exe
#define NTHREADS 1000LLU
static _Atomic long double           sum = 0.0L;

static __attribute((noinline)) void* Kernel(void* input) {
    // enabling optimizations will make this whole block into a noop
    for (size_t i = 0; i < USHRT_MAX; ++i) sum += i;
    return NULL;
}

static pthread_t threads[NTHREADS];
static uint64_t  threadIndices[NTHREADS];

int              main(void) {
    for (size_t i = 0; i < NTHREADS; ++i) threads[i] = i;
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, Kernel, (void*) (threadIndices + i));
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    return EXIT_SUCCESS;
}