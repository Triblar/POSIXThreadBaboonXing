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

// This serves as both the key for the semaphores and the shared memory
// (the same key fits two different locks)

void main()
{



}