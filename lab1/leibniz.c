#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

int NUM_THREADS = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
double pi_approx = 0;

double get_term(double n) {
	double term = 4.0 / (2 * n + 1);
	if ((int)floor(n) & 1) {
		term = 0 - term;
	}
	return term;
}

typedef struct{
	double start;
	double end;
}params;

params * params_new(double start, double end) {
	params * this = malloc(sizeof(params));
	this->start = start;
	this->end = end;
	return this;
}

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


int main(int argc, char ** argv) {

	NUM_THREADS = atoi(argv[1]);
	pthread_t threads[NUM_THREADS];
	double iterations = 5000000;
	double it_per_thread = iterations / (double) NUM_THREADS;
	params ** args = malloc(sizeof(params*) * NUM_THREADS);

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

	for(t=0;t<NUM_THREADS;t++) {
		pthread_join( threads[t], NULL);
		free(args[t]);
	}
	return 0;
}
