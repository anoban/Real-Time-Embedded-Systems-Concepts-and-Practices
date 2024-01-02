#include <fcntl.h> // O_CREAT
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h> // fork
#include <wchar.h>  // Linux requires wchar.h to make the unicode variants of stdio functions available

#include <sys/stat.h>

static volatile uint64_t sumg = 0;
#define ITERMAX 100LLU
static volatile sem_t iosemaphore = { 0 };

// we'll be sharing two semaphores between two processes
// and using one semaphore to control io

int main(void) {
    int    childprocess_id     = 0;

    sem_t *childproc_semaphore = NULL, *parentproc_semaphore = NULL;

    // S_IRWXU - user (owner) has read, write, and execute permission
    childproc_semaphore  = sem_open("cprocsem", O_CREAT, S_IRWXU, 0);
    parentproc_semaphore = sem_open("pprocsem", O_CREAT, S_IRWXU, 0);
    sem_init(&iosemaphore, false, 1);

    sem_wait(&iosemaphore);
    fputws(L"Inside parent process!", stdout);
    sem_post(&iosemaphore);

    if ((childprocess_id = fork()) > 0) { // if the current process is the parent

        for (size_t i = 0; i < ITERMAX; ++i) {
            if (sem_post(childproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_post(&childproc_semaphore) @LINE %d\n", __LINE__);
            for (size_t i = 0; i < ITERMAX; ++i) sumg++;

            sem_wait(&iosemaphore);
            wprintf(L"Parent: gsum is %llu\n", sumg);
            sem_post(&iosemaphore);

            if (sem_wait(parentproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_wait(&parentproc_semaphore) @LINE %d\n", __LINE__);
        }

        if (sem_close(childproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_close(&childproc_semaphore) @LINE %d\n", __LINE__);
        if (sem_close(parentproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_close(&parentproc_semaphore) @LINE %d\n", __LINE__);
        sem_close(&iosemaphore);

        if (sem_unlink("cprocsem") < 0) fwprintf(stderr, L"Error in sem_unlink(\"cprocsem\") @LINE %d\n", __LINE__);
        if (sem_unlink("pprocsem") < 0) fwprintf(stderr, L"Error in sem_unlink(\"pprocsem\") @LINE %d\n", __LINE__);

        return EXIT_SUCCESS;

    } else if (childprocess_id == 0) { // if the current process is the child
        sem_wait(&iosemaphore);
        fputws(L"Inside child process!", stdout);
        sem_post(&iosemaphore);

        for (size_t i = 0; i < ITERMAX; ++i) {
            if (sem_wait(childproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_wait(&childproc_semaphore) @LINE %d\n", __LINE__);
            for (size_t i = 0; i < ITERMAX; ++i) sumg++;

            sem_wait(&iosemaphore);
            wprintf(L"Child: gsum is %llu\n", sumg);
            sem_post(&iosemaphore);

            if (sem_post(parentproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_post(&parentproc_semaphore) @LINE %d\n", __LINE__);
        }

        if (sem_close(childproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_close(&childproc_semaphore) @LINE %d\n", __LINE__);
        if (sem_close(parentproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_close(&parentproc_semaphore) @LINE %d\n", __LINE__);

        return EXIT_SUCCESS;
    } else {
        fputws(L"fork failed", stderr);
        return EXIT_FAILURE;
    }
}