CC = gcc
CFLAGS = -g -Wall -fPIC -pthread  

libscheduler.so: circular_queue.o so_scheduler.o utility.o 
		$(CC) $(CFLAGS) -shared $^ -o $@

so_scheduler.o: so_scheduler.c
		$(CC) $(CFLAGS) -c $<

utility.o: utility.c
		$(CC) $(CFLAGS) -c $<

circular_queue.o: circular_queue.c
		$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
		rm -f *.o *.so