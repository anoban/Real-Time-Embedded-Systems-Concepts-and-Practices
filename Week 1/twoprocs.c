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

int main(void) {
    int               childprocess_id = 0;
    int               stat = 0, return_value = 0;
    pid_t             thisprocess_id  = 0;

    // semaphores
    sem_t *           child_semaphore = NULL, *parent_semaphore = NULL;
    // semaphore names
    const char *const child_name = "sync_child", parent_name = "sync_parent";

    // setting up the semaphores before forking the process
    // sem_t *sem_open(const char *name, int oflag, mode_t mode, unsigned int value);

    // S_IRWXU - user (owner) has read, write, and execute permission
    child_semaphore  = sem_open(child_name, O_CREAT, S_IRWXU, 0);
    parent_semaphore = sem_open(parent_name, O_CREAT, S_IRWXU, 0);

    // fork the current process, creates a copy of the parent process
    // post-fork we could be the child process or the parent process.
    // the return value of fork() can be used to determine whether the current process is deemed parent or child.
    // 0 - child, non zero - parent

    // intentional assignment
    if (chilprocess_id = fork()) { // parent

    } else {                       // child
    }
}