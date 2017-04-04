/*
 * James Dempsey - jed117
 * EECS Assignment 7
 * Baboon Crossing Using POSIX Threads
 * Adapted from the solutions to POSIX Thread problems from http://eecs-002.case.edu/338.S17/old%20assignments%20and%20exams.html
 */

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
 // The amount of loop iterations to stall between making baboons
 #define BABOON_CREATE_STALL_TIME	7000

 extern const unsigned int RAND_RANGE;

 typedef struct _thread_data_t {
 	int tid;//thread id
 } thread_data_t;

const unsigned int RAND_RANGE = RAND_MAX>>10;

int xingCount = 0;
int xedCount = 0;
int toBWaitCount = 0;
int toAWaitCount = 0;
typedef enum {None, DirToB, DirToA} crossDirection;
crossDirection xingDirection = None;
sem_t a, b, mutex; // a is cross to A semaphore, b is cross to B semaphore
time_t t;
char debug = 1;//used to debug

int getRand();//returns a random int
void *crossToA(void *arg);//thread function for crossing to A
void *crossToB(void *arg);//thread function for crossing to B
void semwait(sem_t *sem);//error-checked semaphore wait
void semsignal(sem_t *sem);//error-checked semaphore signal

int getRand() {
	return ((rand() % RAND_RANGE) - RAND_RANGE/2);
}

// Stall function for crossing Rope
void stall(int iterations){
	int i;
	for(i = 0; i < iterations; i++){
		;
	}
}

// Error checked semaphore wait function
void semwait(sem_t *sem) {
	if (sem_wait(sem) < 0) {
		perror("sem_wait");
		exit(EXIT_FAILURE);
	}
}

// Error checked semaphore signal function
void semsignal(sem_t *sem) {
	if (sem_post(sem) < 0) {
		perror("sem_post");
		exit(EXIT_FAILURE);
	}
}

void *crossToA(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;
	printf("Thread id: %d CrossToA has been called!.\n", data->tid);
	printf("Thread id: %d CrossToA Waiting on Mutex.\n", data->tid);
	semwait(&mutex);
	printf("Thread id: %d CrossToA Passed Mutex.\n", data->tid);

	// Implement logic here for Baboon Crossing problem specifics
	if((xingDirection == DirToA || xingDirection == None) && xingCount < 4 &&
		(xedCount + xingCount < 10 || toBWaitCount == 0) && toAWaitCount == 0)
	{
		printf("Thead id: %d CrossToA is crossing now!\n", data->tid);
		xingDirection = DirToA;
		xingCount++;
		printf("Thread id: %d CrossToA Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
	}
	else
	{
		printf("Thread id: %d CrossToA is waiting.\n", data->tid);
		toAWaitCount++;
		printf("Thread id: %d CrosstoA Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
		semwait(&a);
		printf("Thread id: %d CrossToA was waiting, now im signaled.\n", data->tid);
		toAWaitCount--;
		xingCount++;
		xingDirection = DirToA;

		// *** Baboons check to see if anyone is waiting behind them, and signal those behind them
		// *** This is necessary because otherwise only one baboon would be on the rope at a time. Symmetrical with logic in crossToB
		if(toAWaitCount > 0 && xingCount < 4 && (xedCount + xingCount < 10 || toBWaitCount == 0))
		{
			printf("Thread id: %d CrossToA is going to cross.\n", data->tid);
			printf("Thread id: %d CrossToA Signalling thread behind me.\n", data->tid);
			semsignal(&a);
		}
		else
		{
			printf("Thread id: %d CrossToA is going to cross.\n", data->tid);
			printf("Thread id: %d CrossToA Signaling Mutex.\n", data->tid);
			semsignal(&mutex);
		}
	}

	printf("Thread id: %d CrossToA is crossing the rope.\n", data->tid);
	// Time to cross rope
	stall(CROSS_ROPE_STALL_TIME);

	printf("Thread id: %d CrossToA crossed- Waiting for Mutex.\n", data->tid);
	semwait(&mutex);
	printf("Thread id: %d CrossToA Passed Mutex.\n", data->tid);
	xedCount++;
	xingCount--;

	if(toAWaitCount != 0 && (xingCount + xedCount < 10 || toBWaitCount == 0))
	{
		printf("Thread id: %d CrossToA signaling a waiting CrossToA.\n", data->tid);
		semsignal(&a);
	}
	else if (xingCount == 0 && toBWaitCount != 0 &&
		(toAWaitCount == 0 || xedCount + xingCount >= 10))
	{
		printf("Thread id: %d CrossToA changing direction to DirToB.\n", data->tid);
		xingDirection = DirToB;
		xedCount = 0;
		printf("Thread id: %d CrossToA signaling a waiting CrossToB.\n", data->tid);
		semsignal(&b);
	}
	else if(xingCount == 0 && toBWaitCount == 0 && toAWaitCount == 0)
	{
		printf("Thread id: %d CrossToA setting direction to none.\n", data->tid);
		xingDirection = None;
		xedCount = 0;
		printf("Thread id: %d CrossToA Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
	}
	else
	{
		printf("Thread id: %d CrossToA Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
	}

	fflush(stdout);
	pthread_exit(NULL);
}

void *crossToB(void *arg) {
	thread_data_t *data = (thread_data_t *)arg;
	printf("Thread id: %d CrossToB has been called!.\n", data->tid);
	printf("Thread id: %d CrossToB Waiting on Mutex.\n", data->tid);
	semwait(&mutex);
	printf("Thread id: %d CrossToB Passed Mutex.\n", data->tid);

	if((xingDirection == DirToB || xingDirection == None) && xingCount < 4 &&
		(xedCount + xingCount < 10 || toAWaitCount == 0) && toBWaitCount == 0)
	{
		printf("Thead id: %d CrossToB is crossing now!\n", data->tid);
		xingDirection = DirToB;
		xingCount++;
		printf("Thread id: %d CrossToB Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
	}
	else
	{
		printf("Thread id: %d CrossToB is waiting.\n", data->tid);
		toBWaitCount++;
		printf("Thread id: %d CrosstoB Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
		semwait(&b);
		printf("Thread id: %d CrossToB was waiting, now im signaled.\n", data->tid);
		toBWaitCount--;
		xingCount++;
		xingDirection = DirToB;

		// *** Baboons check to see if anyone is waiting behind them, and signal those behind them
		// *** This is necessary because otherwise only one baboon would be on the rope at a time. Symmetrical with logic in crossToA
		if(toBWaitCount > 0 && xingCount < 4 && (xedCount + xingCount < 10 || toAWaitCount == 0))
		{
			printf("Thread id: %d CrossToB is going to cross.\n", data->tid);
			printf("Thread id: %d CrossToB Signalling thread behind me.\n", data->tid);
			semsignal(&b);
		}
		else
		{
			printf("Thread id: %d CrossToB is going to cross.\n", data->tid);
			printf("Thread id: %d CrossToB Signaling Mutex.\n", data->tid);
			semsignal(&mutex);
		}
	}

	printf("Thread id: %d CrossToB is crossing the rope.\n", data->tid);
	// Time to cross rope
	stall(CROSS_ROPE_STALL_TIME);

	printf("Thread id: %d CrossToB crossed- Waiting for Mutex.\n", data->tid);
	semwait(&mutex);
	printf("Thread id: %d CrossToB Passed Mutex.\n", data->tid);
	xedCount++;
	xingCount--;

	if(toBWaitCount != 0 && (xingCount + xedCount < 10 || toAWaitCount == 0))
	{
		printf("Thread id: %d CrossToB signaling a waiting CrossToA.\n", data->tid);
		semsignal(&b);
	}
	else if (xingCount == 0 && toAWaitCount != 0 &&
		(toBWaitCount == 0 || xedCount + xingCount >= 10))
	{
		printf("Thread id: %d CrossToB changing direction to DirToA.\n", data->tid);
		xingDirection = DirToA;
		xedCount = 0;
		printf("Thread id: %d CrossToB signaling a waiting CrossToA.\n", data->tid);
		semsignal(&a);
	}
	else if(xingCount == 0 && toBWaitCount == 0 && toAWaitCount == 0)
	{
		printf("Thread id: %d CrossToB setting direction to none.\n", data->tid);
		xingDirection = None;
		xedCount = 0;
		printf("Thread id: %d CrossToB Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
	}
	else
	{
		printf("Thread id: %d CrossToB Signaling Mutex.\n", data->tid);
		semsignal(&mutex);
	}


	fflush(stdout);
	pthread_exit(NULL);
}
/* Main function. From command line call ./as7.o aabbbbaaabbbababaaaabbbaababa
* This call is case insenstivie and for each a or b will create a thread having
* a baboon cross to a and b respectively
*/
int main(int argc, char const *argv[])
{
	pthread_t threads[NUM_THREADS];
	thread_data_t thread_data[NUM_THREADS];
	int errorCheck;//used to error check thread creation
	//initialize semaphores
	if (sem_init(&mutex, 0, (unsigned int)1) < 0
		|| sem_init(&a, 0, (unsigned int)0) < 0
		|| sem_init(&b, 0, (unsigned int)0) < 0)
    {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	if (debug)
    {
		/*
		* debug mode:
		* Create baboon crossing to A
		* Create baboon crossing to B
		* Create baboon crossing to B
		*/
		thread_data[0].tid = 0;
		pthread_create(&threads[0], NULL, crossToA, &thread_data[0]);

		thread_data[1].tid = 1;
		pthread_create(&threads[1], NULL, crossToB, &thread_data[1]);

		thread_data[2].tid = 2;
		pthread_create(&threads[2], NULL, crossToB, &thread_data[2]);

		pthread_join(threads[0], NULL);
		pthread_join(threads[1], NULL);
		pthread_join(threads[2], NULL);
	}
    else
    {
        int i = 0;
        while(argv[1][i] != 0)
        {
			thread_data[i].tid = i;//set thread id to current i value
            switch (argv[1][i]) {
    			case 'a':
    			case 'A':
                    pthread_create(&threads[i], NULL, crossToA, &thread_data[i]);
    				break;

    			case 'b':
    			case 'B':
    				pthread_create(&threads[i], NULL,  crossToB, &thread_data[i]);
    				break;

    			default:
    				printf("!!!Invalid argument!!!\n");
    				exit(EXIT_FAILURE);
    				break;

    		}
            // Stall between each baboon creation (stall length is a bit less than the time it takes to cross the rope)
            stall(BABOON_CREATE_STALL_TIME);
            i++;
		}
        // Join all threads
        for(int j = 0; j < i; j++)
        {
            pthread_join(threads[j], NULL);
        }
	    return EXIT_SUCCESS;
    }

}
