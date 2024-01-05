-------
### ___Using WSL2 Debian Bullseye on Windows 11 machine to learn the materials and complete the assignments, without the recommended Raspberry Pi 3b+, let's see how this goes :)___
-------

### ___Learning Objectives:___
-----------------

1. Options for Real-Time systems:
    - Cyclic Executives (CE)
    - Real Time Operating Systems (RTOS)
    - ___`Traditional POSIX OSes with real time extensions`___ - Approach adpoted in this course since it is the most convenient.
      
2. Terminology, challenges, timing constraints & performance.

3. Rate Monotonic Concepts:
   - Best Effort (BE) systems
   - Soft Real Time (SRT) systems
   - Hard Real Time (HRT) systems

4. RM scheduling practices:
   - Rate Monotonic Least Upper Bound (RM LUB)
   - Timing analyses
   - POSIX RT extensions (found in FreeBSD, Linux, Solaris etc,)
  
5. Priority Preemptive, Run-to-completion scheduling (different from fair scheduling and called SCHED_FIFO in Linux RT extensions)

6. Interference & blocking and their challenges.

7. Fixed priority & dynamic priority options for services.

8. Coding practices on Linux with POSIX RT extensions.

9. Use of Rate Monotonic Analyses (RMA) on multi-core systems.

10. Methods of test and verification.