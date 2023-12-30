// a minimal example of using semaphores
// semaphores doesn't protect a resource from data races themselves.
// we just leverage the nature of semaphore's synchronized serial access to our benefit.
// by sandwiching the code prone to data races between a sem_wait() and a sem_post()
// which indirectly allows us to control the parallel access to the resource of concern as a corollary benefit

#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NTHREADS     10000LLU
#define SECTONANOSEC 1000000000LLU

static volatile sem_t  global_semaphore  = { 0 };
static volatile size_t sum               = 0;
static pthread_t       threads[NTHREADS] = { 0 }; // pthread_t is just a thread Id, akin to process Ids

// pthread exec
static void* __attribute((noinline)) change(void* input /* NULL */) {
    sum++;
    return NULL;
}

// an alternative, policed by a semaphore
static void* __attribute((noinline)) change_policed(void* input /* NULL */) {
    // when a new thread is launched, it will send a wait message, which will decrement the semaphore's value
    // MIND YOU THIS WILL JUST DECREMENT NOT ZERO THE VALUE, IN ORDER FOR THE VALUE TO BE ZEROED, IT MUST HAVE BEEN 1 AT THE TIME OF ACCESS.
    sem_wait(&global_semaphore); // if the value is 0, sum++ will not be executed until it becomes > 0
    // whis has to be done by some other thread or process by sending a post message.
    // if it was > 0, this thread will happily increment sum
    sum++;
    sem_post(&global_semaphore); // this increments the semaphore's value.

    // imagine the case when the value held by the semaphore is 1 at the time of a thread launch, this thread will then decrement the value
    // to 0, thus effectively preventing any other threads from modifying the semaphore's value. with this guard in place, this thread can
    // increment the sum, granted that no other thread will try to access it! once sum is incremented, the thread will decrement the
    // semaphore's value

    return NULL;
}

int main(void) {
    // unregulated parallelism
    size_t          elapsed_ns  = 0;
    struct timespec tspec_start = { 0 }, tspec_stop = { 0 };

    clock_gettime(CLOCK_MONOTONIC, &tspec_start);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, change, NULL);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &tspec_start);

    elapsed_ns = (tspec_stop.tv_sec * SECTONANOSEC + tspec_stop.tv_nsec) - (tspec_start.tv_sec * SECTONANOSEC + tspec_start.tv_nsec);
    printf(
        "After parallel execution of %llu threads to increment the sum, the value is %zu {%zu nanoseconds}\n", NTHREADS, sum, elapsed_ns
    );

    // parallellsim, synchronized by a semaphore.
    // semaphore is like a safe, with a controlled access to it's value.
    // we specify an initial value when initializing a semaphore.
    // as long as this value is 0, no other thread or process can modify the value.

    // the value we initialize the semaphore has decisive effect on how many threads get to access the protected resource simultaneously.
    // 1 will only allow one access
    sum = 0;
    sem_init(&global_semaphore, false /* semaphore isn't shared between processes */, 1);

    clock_gettime(CLOCK_MONOTONIC, &tspec_start);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_create(threads + i, NULL, change_policed, NULL);
    for (size_t i = 0; i < NTHREADS; ++i) pthread_join(threads[i], NULL);
    clock_gettime(CLOCK_MONOTONIC, &tspec_start);

    elapsed_ns = tspec_stop.tv_nsec - tspec_start.tv_nsec;
    printf(
        "After synchronized parallel execution of %llu threads to increment the sum, the value is %zu {%zu nanoseconds}\n",
        NTHREADS,
        sum,
        elapsed_ns
    );

    return EXIT_SUCCESS;
}