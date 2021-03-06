#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef NUM_THREADS
#define NUM_THREADS 200
#endif

#define CROSS_ROPE_STALL_TIME	100000

extern const unsigned int RAND_RANGE;

typedef struct _thread_data_t {
	int tid;//thread id
} thread_data_t;
