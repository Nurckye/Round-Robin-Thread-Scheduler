# Round-Robin Thread Scheduler
A thread scheduler based on the Round-Robin preemptive planning algorithm, written in C as a shared library for Linux systems which controls the execution of threads in user-space. It uses the POSIX threads library. Written as a

The simulated environment uses a virtual time system that 'ticks' every time a logical instruction happens - the defined logical instructions are as follows: 

-ts_init
-ts_fork
-ts_wait
-ts_signal
-ts_exec
-ts_end
