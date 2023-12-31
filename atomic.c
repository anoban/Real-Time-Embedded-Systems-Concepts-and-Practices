#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>

#define ITERMAX  1000LLU
#define NTHREADS 1000LLU

// atomic type qualifier specifies that the compiler should treat all operations on the select variable as atomic operations.
// atomic types will work will all parallel execution implementations, not confined to pthreads

// _Atomic types cannot be annotated with the volatile qualifier
static _Atomic uint64_t  atomic_sum        = 0;
static volatile uint64_t sum               = 0;
static pthread_t         threads[NTHREADS] = { 0 };

static void* __attribute((noinline)) kernel(void* param) {
    for (size_t i = 0; i < ITERMAX; ++i) {
        sum++;        // no protection here
        atomic_sum++; // the operation here is a simple atomic incrementation
    }
    return NULL;
}

int main(void) {
    for (size_t i = 0; i < ITERMAX; i++) threads[i] = i;
    for (size_t i = 0; i < ITERMAX; i++) pthread_create(threads + i, NULL, kernel, NULL);
    for (size_t i = 0; i < ITERMAX; i++) pthread_join(threads[i], NULL);
    wprintf_s(L"Sum is %llu and _Atomic sum is %llu\n", sum, atomic_sum);
    return 0;
}