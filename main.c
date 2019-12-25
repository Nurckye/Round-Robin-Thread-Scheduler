#include <stdio.h>
#include <unistd.h>
#include "circular_queue.h"
#include "so_scheduler.h"
#include "utility.h"


void test_sched_handler_12(unsigned int dummy)
{
	unsigned int i;
    ts_wait(3);
	for (i = 0; i < 10; i++) {
		printf("[i] : %d , [time] : %d\n", i, time_counter);
        fflush(stdin);
		ts_exec();
	}
    printf("END OF EXECUTION >>>\n");
}

void test_sched_handler_12_1(unsigned int dummy)
{
	unsigned int i;
	for (i = 0; i < 10; i++) {
		printf("[i] : %d , [time] : %d\n", i, time_counter);
        fflush(stdin);
		ts_exec();
	}
    so_signal(3);
    printf("END OF EXECUTION >>>\n");
}

int main()
{
    printf("STARTED\n");
    ts_init(2, 15);
    ts_fork(test_sched_handler_12, 3);
    ts_fork(test_sched_handler_12_1, 3);
    // ts_fork(test_sched_handler_12, 3);
    // ts_fork(test_sched_handler_12, 3);
    // ts_fork(test_sched_handler_12, 3);
    // ts_fork(test_sched_handler_12, 3);
    ts_end();
    return 0;
}