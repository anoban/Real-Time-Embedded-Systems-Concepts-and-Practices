#include <wchar.h>  // Linux requires wchar.h to make the unicode variants of stdio functions available
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>

// In mission critical systems, the code must be clean, functionally correct and efficient.
// Processes will at least have one thread, and IO descriptors / handles / indices / channels
// and protected memory

// Processes are very different from threads.
// To create a process you need a parent thread
// And to create a child process you need a parent process
// We can create as many threads as we want inside a container

// Generally process threads are created user space which is more forgiving
// Threads can be created in kernel space too.

static volatile uint64_t sumg = 0;
#define ITERMAX 100LLU

// we'll be sharing two semaphores between two processes

int main(void) {
    int    childprocess_id     = 0;

    // semaphores
    sem_t *childproc_semaphore = NULL, *parentproc_semaphore = NULL;

    // setting up the semaphores before forking the process
    // sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);

    // S_IRWXU - user (owner) has read, write, and execute permission
    childproc_semaphore  = sem_open("cprocsem", O_CREAT, S_IRWXU, 0);
    parentproc_semaphore = sem_open("pprocsem", O_CREAT, S_IRWXU, 0);

    // fork the current process, creates a copy of the parent process
    // post-fork we could be the child process or the parent process.
    // the return value of fork() can be used to determine whether the current process is deemed parent or child.
    // 0 - child, non zero - parent

    // both semaphores will be alternating ITERMAX times
    // intentional assignment
    if ((childprocess_id = fork()) > 0) { // if the current process is the parent

        fputws(L"Inside parent process!",stdout);
        for (size_t i = 0; i < ITERMAX; ++i) {
            // open child process's semaphore, this statement needs to execute in order for the sem_wait(&childproc_semaphore) of child proc
            // to execute
            if (sem_post(childproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_post(&childproc_semaphore) @LINE %d\n", __LINE__);
            for (size_t i = 0; i < ITERMAX; ++i) sumg++;
            // close this (parent) process's semaphore
            if (sem_wait(parentproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_wait(&parentproc_semaphore) @LINE %d\n", __LINE__);
        }

        wprintf(L"gsum is %llu: Message from parent\n", sumg);

        // parent proc was the one that created the semaphores, so it has to close and unlink those semaphores!
        if (sem_close(childproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_close(&childproc_semaphore) @LINE %d\n", __LINE__);
        if (sem_close(parentproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_close(&parentproc_semaphore) @LINE %d\n", __LINE__);

        if (sem_unlink("cprocsem") < 0) fwprintf(stderr, L"Error in sem_unlink(\"cprocsem\") @LINE %d\n", __LINE__);
        if (sem_unlink("pprocsem") < 0) fwprintf(stderr, L"Error in sem_unlink(\"pprocsem\") @LINE %d\n", __LINE__);

        return EXIT_SUCCESS;

    } else if(childprocess_id == 0) {                                   // if the current process is the child

        fputws(L"Inside child process!",stdout);
        for (size_t i = 0; i < ITERMAX; ++i) { // close and open semaphores ITERMAX times iteratively
            // we're locking the child process's semaphore, whose value is initially zero
            // unless another thread or process increments its value, this statement won't execute
            if (sem_wait(childproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_wait(&childproc_semaphore) @LINE %d\n", __LINE__);
            for (size_t i = 0; i < ITERMAX; ++i) sumg++;
            // we are opening the parent process's semaphore, whose value becomes 1
            // this statement needs to be executed in order for the parent proc's sem_wait(&parentproc_semaphore) to be executed
            if (sem_post(parentproc_semaphore) < 0) fwprintf(stderr, L"Error in sem_post(&parentproc_semaphore) @LINE %d\n", __LINE__);
        }

        wprintf(L"gsum is %llu: Message from child\n", sumg);

        return EXIT_SUCCESS;
    }else{
        fputws(L"fork failed",stderr);
        return EXIT_FAILURE;
    }
}