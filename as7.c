/*
 * James Dempsey - jed117
 * EECS Assignment 7
 * Baboon Crossing Using POSIX Threads
 * Adapted from the solutions to POSIX Thread problems from http://eecs-002.case.edu/338.S17/old%20assignments%20and%20exams.html
 */

#include <baboon.h>

const unsigned int RAND_RANGE = RAND_MAX>>10;

int xingCount = 0;
int xedCount = 0;
int toBWaitCount = 0;
int toAWaitCount = 0;
enum {None, DirToB, DirToA} xingDirection;
xingDirection = None;
sem_t a, b, mutex; // a is cross to A semaphore, b is cross to B semaphore
time_t t;
char debug = 0;//used to debug

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
	semwait(&mutex)
	printf("Thread id: %d CrossToA Passed Mutex.\n", data->pid);
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
	semwait(&mutex)
	printf("Thread id: %d CrossToB Passed Mutex.\n", data->pid);
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

int main(int argc, char const *argv[])
{
	pthread_t threads[NUM_THREADS];
	thread_data_t thread_data[NUM_THREADS];
	int errorCheck;//used to error check thread creation

	//seed the random number generator
	srand((unsigned int)time(&t));

	//initialize semaphores
	if (sem_init(&mutex, 0, (unsigned int)1) < 0
		|| sem_init(&a, 0, (unsigned int)0) < 0
		|| sem_init(&b, 0, (unsigned int)0) < 0) {
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	if (debug) {
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
	} /* else {
		for (int i = 0; i < NUM_THREADS; ++i) {

			void *thread_func;//the function to call

			thread_data[i].tid = i;//set thread id to current i value

			if ((thread_data[i].amount = getRand()) < 0) {//if random amount < 0
				thread_data[i].amount = -(thread_data[i].amount);//negate it
				thread_func = withdraw;//make this a withdrawal
			} else {//else amount > 0
				thread_func = deposit;//make this a deposit
			}
			if ((errorCheck = pthread_create(&threads[i], NULL, thread_func, &thread_data[i]))) {
				fprintf(stderr, "error: pthread_create, %d\n", errorCheck);
				return EXIT_FAILURE;
			}
		}

		/*	make a large final deposit to ensure that any waiting withdrawal threads finish
		*
		*	example:
		*
		*	4 threads:
		*
		*	1. withdraw 100
		*	2. withdraw 100
		*	3. withdraw 100
		*	4. deposit  200
		*
		*	final thread: deposit 1,000,000 ... ensures thread 3 finishes


		sleep(1);
		printf("...............FINAL THREAD (%d) COMING.............", NUM_THREADS+1);
		fflush(stdout);
		sleep(1);
		printf(".");
		fflush(stdout);
		sleep(1);
		printf(".");
		fflush(stdout);
		sleep(1);
		printf(".");
		fflush(stdout);
		sleep(1);
		printf("\n");
		fflush(stdout);
		pthread_t finalThread;
		thread_data_t ftd = { NUM_THREADS + 1, RAND_MAX/2};//final thread data
		if ((errorCheck = pthread_create(&finalThread, NULL, deposit, &ftd))) {
			fprintf(stderr, "error: pthread_create, %d\n", errorCheck);
				return EXIT_FAILURE;
		}

		for (int i = 0; i < NUM_THREADS; ++i) {
			if ((errorCheck = pthread_join(threads[i], NULL))) {
				fprintf(stderr, "error: pthread_join, %d\n", errorCheck);
			}
		}

		if ((errorCheck = pthread_join(finalThread, NULL))) {
			fprintf(stderr, "error: pthread_join, %d\n", errorCheck);
		}
	}
	*/

	return EXIT_SUCCESS;
}
