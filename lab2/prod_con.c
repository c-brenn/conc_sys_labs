#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef enum { FALSE, TRUE} boolean;

typedef struct {
	int n;
	int size;
	int num_to_produce;
	boolean finished;
}buffer;

buffer * buffer_new(int size, int num_to_produce) {
	buffer * this = malloc(sizeof(buffer));
	this->n = 0;
	this->num_to_produce = num_to_produce;
	this->size = size;
	this->finished = FALSE;
	return this;
}

void check_results(char * string, int val){          
	if(val) {
		printf("Failed with %d at %s", val, string);
		exit(1);
	}                                       
}

void print_buffer(buffer * buff, char * s) {
	fprintf(stdout, "%s ||", s);
	int i;
	for(i = 0; i < buff->size; i++) {
		if(i < buff->n) {
			fprintf(stdout, "[]");
		}
		if(i < buff->size - 1) {
			fprintf(stdout, ", ");
		}
	}
	fprintf(stdout, "||\n");
}

void do_work() {
	float t = (float)rand() / (float) RAND_MAX;
	t = 5 * t;
	sleep((int) t);

}

boolean full = FALSE;
boolean empty  = TRUE;
pthread_mutex_t buff_mutex =  PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t consumer_cond = PTHREAD_COND_INITIALIZER;

void * produce(void* buff) {
	buffer * b = (buffer*) buff;
	int rc = pthread_mutex_lock(&buff_mutex);
	check_results("pthread_mutex_lock() producer\n", rc);
	while(!b->finished) {
		while(full && !b->finished) {
			rc = pthread_cond_wait(&producer_cond, &buff_mutex);
			if (rc) {
				printf("Producer condwait failed, rc=%d\n", rc);
				pthread_mutex_unlock(&buff_mutex);
				exit(1);
			}
		}
		if(b->num_to_produce-- == 0) {
			b->finished = TRUE;
			printf("Producer has finished producing.\n");
		} else {
			b->n++;
			print_buffer(b, "Produce");
			empty = FALSE;
			if(b->n == b->size) {
				full = TRUE;
			}
		}
		rc = pthread_mutex_unlock(&buff_mutex);
		check_results("Producer mutex_unlock()\n", rc);
		rc = pthread_cond_signal(&consumer_cond);
		do_work();
	}
	printf("Producer exit.\n");
	pthread_exit(0);
}

void * consume(void* buff) {
	buffer * b = (buffer*) buff;
	int rc = pthread_mutex_lock(&buff_mutex);
	check_results("pthread_mutex_lock() consume\n", rc);
	while(!b->finished || b->n > 0) {
		while(empty && !b->finished) {
			rc = pthread_cond_wait(&consumer_cond, &buff_mutex);
			if (rc) {
				printf("Consumer condwait failed, rc=%d\n", rc);
				pthread_mutex_unlock(&buff_mutex);
				exit(1);
			}
		}
		if (b->n > 0) {
			b->n--;
			print_buffer(b, "Consume");
			full = FALSE;
			if(b->n == 0) {
				empty = TRUE;
			}
		}
		rc = pthread_mutex_unlock(&buff_mutex);
		check_results("Consumer mutex_unlock()\n", rc);
		rc = pthread_cond_signal(&producer_cond);
		do_work();
	}
	printf("Consumer exit.\n");
	pthread_exit(0);
}
 


int main(int argc, char ** argv){
	printf("Creating buffer\n");
	buffer * buff = buffer_new(10, 10);
	// parse num_threads from args
	int num_consumers = atoi(argv[1]);
	pthread_t consumers[num_consumers];
	pthread_t producers[num_consumers];
	printf("Creating producer...\n");
	int i;
	for(i = 0; i < num_consumers; i++) {
		printf("Creating Producer: %d\n", i);
		int return_code = pthread_create(&producers[i],NULL,
									produce,(void *)buff);
		check_results("pthread_create() consumer\n", return_code);
		printf("Creating consumer: %d\n", i);
		return_code = pthread_create(&consumers[i],NULL,
									consume,(void *)buff);
		check_results("pthread_create() producer\n", return_code);
	}
	for(i = 0; i < num_consumers; i++) {
		pthread_join(producers[i], NULL);
		pthread_join(consumers[i], NULL);
	}
	print_buffer(buff, "Done: ");
	return 0;	
}