#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>

#ifdef __STDC_NO_ATOMICS__
    #error "Compiler doesn't support _Atomic s"
#endif

#define ITERMAX  1000LLU
#define NTHREADS 1000LLU

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

// gcc might not be able to map complex operations to atomic instructions, but it depends
static void* __attribute((noinline)) complexkernel(void* param) {
    for (size_t i = 0; i < ITERMAX; ++i) {
        sum--;
        atomic_sum = i + (uint64_t) cos(M_PI * i); // suprisingly gcc 13 does manage to translate this statement into atomic instructions
        // _Atomic qualifier asks for ISA level safety gurantees, which are processor specific hardware instructions
        // unlike software level guardrails (locks, semaphores and mutexes)
    }
    return NULL;
}

// _Atomic instructions can be extremely efficient in comparison to software level guardrails
// atomicity is completely dependent on CPU architectures.
// may fail in systems that doesn't support atomic instructions.
// and C standard allows implementations to decide whether to support _Atomics
// i.e <stdatomic.h> is an optional header and is only supported in revisions from and after C11

int main(void) {
    for (size_t i = 0; i < ITERMAX; i++) threads[i] = i;
    for (size_t i = 0; i < ITERMAX; i++) pthread_create(threads + i, NULL, kernel, NULL);
    for (size_t i = 0; i < ITERMAX; i++) pthread_join(threads[i], NULL);
    wprintf_s(L"Sum is %llu and _Atomic sum is %llu\n", sum, atomic_sum);

    for (size_t i = 0; i < ITERMAX; i++) pthread_create(threads + i, NULL, complexkernel, NULL);
    for (size_t i = 0; i < ITERMAX; i++) pthread_join(threads[i], NULL);
    wprintf_s(L"Sum is %llu and _Atomic sum is %llu\n", sum, atomic_sum);

    return 0;
}