#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

//	mutex and the global that it protects
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
double pi_approx = 0;

//	calculates the nth term in the gregory leibniz series (outlined below)
double get_term(double n) {
	double term = 4.0 / (2 * n + 1);
	if ((int)floor(n) & 1) {
		term = 0 - term;
	}
	return term;
}

/*	
	this struct contains the parameters that each
	thread uses to do it's share of the work
*/
typedef struct{
	double start;
	double end;
}params;

//	creates a new params struct pointer
params * params_new(double start, double end) {
	params * this = malloc(sizeof(params));
	this->start = start;
	this->end = end;
	return this;
}

/*	
	this is the function that each of the threads calls
	this function calculates the terms from 'start' to 'end'
	in the gregory leibniz series for caculating pi.
	http://www.wikiwand.com/en/Leibniz_formula_for_%CF%80
	the series is as follows:

	π=4∑ (−1)^k * (1 / (2k+1)) 
*/
void * leibniz(void* args) {
	params * p = (params*) args;
	double start = p->start;
	double end = p->end;
	double sum = 0;
	for(; start < end; start++) {
		sum += get_term(start);
	}
	pthread_mutex_lock(&mut);
	pi_approx += sum;
	pthread_mutex_unlock(&mut);
	pthread_exit(0);
}

/*
	The program always calculates pi using the first 'iterations'
	terms of the leibniz series. The calculating of the terms is spread
	among n threads, where n is an int passed as a command line arg
*/
int main(int argc, char ** argv) {

	// parse num_threads from args
	int num_threads = atoi(argv[1]);
	pthread_t threads[NUM_THREADS];
	double iterations = 5000000;

	// cacluate iterations per thread
	double it_per_thread = iterations / (double) num_threads;
	params ** args = malloc(sizeof(params*) * num_threads);

	// create num_threads threads and start them working
	int t, rc;
	for (t=0;t<NUM_THREADS;t++) {
		double start = (double) t * it_per_thread;
		double end = start + it_per_thread;
		args[t] = params_new(start, end);
		rc = pthread_create(&threads[t],NULL,
		leibniz,(void *)args[t]);
		if (rc) {
			exit(-1);
		}
	}
	// wait for the threads to join
	for(t=0;t<NUM_THREADS;t++) {
		pthread_join( threads[t], NULL);
		free(args[t]);
	}
	return 0;
}
