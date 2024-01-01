#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

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

int main(void) {
    int               childprocess_id = 0;
    int               stat = 0, return_value = 0;

    // semaphores
    sem_t *           childproc_semaphore = NULL, *parentproc_semaphore = NULL;
    // semaphore names
    const char *const child_name = "sync_child", parent_name = "sync_parent";

    // setting up the semaphores before forking the process
    // sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);

    // S_IRWXU - user (owner) has read, write, and execute permission
    childproc_semaphore  = sem_open(child_name, O_CREAT, S_IRWXU, 0);
    parentproc_semaphore = sem_open(parent_name, O_CREAT, S_IRWXU, 0);

    // fork the current process, creates a copy of the parent process
    // post-fork we could be the child process or the parent process.
    // the return value of fork() can be used to determine whether the current process is deemed parent or child.
    // 0 - child, non zero - parent

    // intentional assignment
    if (chilprocess_id = fork()) { // if the current process is the parent

    } else {                       // if the current process is the child
        _putws(L"Inside child process!");
        for (size_t i = 0; i < ITERMAX; ++i) {
            if (sem_wait(&childproc_semaphore) < 0) fputws(L"Error in sem_wait()", stderr);
            for (size_t i = 0; i < ITERMAX; ++i) sumg++;
            if (sem_post(&childproc_semaphore) < 0) fputws(L"Error in sem_post()", stderr);
        }
        sem_close(&childproc_semaphore);
    }
}