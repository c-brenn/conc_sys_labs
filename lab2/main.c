#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

typedef enum { FALSE, TRUE} boolean;

pthread_mutex_t data_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t	data_cond = PTHREAD_COND_INITIALIZER;
int data_present = 0;

void check_results(char * string, int val){             
	if(val) {
		printf("Failed with %d at %s", val, string);
		exit(1);
	}                                       
}

// buffer to hold items yet to be consumed
typedef struct {
	int n;
	int done;
}buffer;

buffer * buffer_new() {
	buffer * this = malloc(sizeof(buffer));
	this->n = 0;
	this->done = FALSE;
	return this;
}

void buffer_free(buffer * this) {
	free(this);
	return;
}

void * consume(void* args) {
	printf("Consumer\n");
	buffer * buff = (buffer *) args;
	int rc = pthread_mutex_lock(&data_mutex);
	check_results("pthread_mutex_lock() consumer\n", rc);
	int done = buff->done;
	while (!done) {
		while (!data_present) {
			printf("Consumer waiting for data...\n");
			rc = pthread_cond_wait(&data_cond, &data_mutex);
			if (rc) {
				printf("Consumer condwait failed, rc=%d\n", rc);
				pthread_mutex_unlock(&data_mutex);
				exit(1);
			}
		}
		printf("Consumer has data to consume\n");
		buff->n--;
		done = buff->done;
		rc = pthread_mutex_unlock(&data_mutex);
		check_results("pthread_mutex_unlock() consumer\n", rc);
		sleep(2);
	}
	printf("Consumer has finish consuming all data\n");
	pthread_exit(0);
}

void * produce(void* args) {
	printf("Producer\n");
	buffer * buff = (buffer*) args;
	int rc;
	int i = 2;
	while (i > 0) {
		rc = pthread_mutex_lock(&data_mutex);
		check_results("pthread_mutex_lock() producer\n", rc);
		buff->n++;
		rc = pthread_cond_signal(&data_cond);
		if (rc){
			pthread_mutex_unlock(&data_mutex);
			check_results("Failed to wake up consumer, rc=%d\n", rc);
			exit(1);			
		}
		rc = pthread_mutex_unlock(&data_mutex);
		check_results("pthread_mutex_lock() producer\n", rc);
		i--;
		data_present = 1;
		printf("Created item\n");
		sleep(2);
	}
	printf("Producer has finished producing, setting done variable\n");
	rc = pthread_mutex_lock(&data_mutex);
	check_results("pthread_mutex_lock() producer\n", rc);
	buff->done = TRUE;
	pthread_mutex_unlock(&data_mutex);
	pthread_exit(0);
}


int main(int argc, char ** argv) {

	// create buffer
	printf("Creating buffer\n");
	buffer * buff = buffer_new();
	// parse num_threads from args
	int num_consumers = atoi(argv[1]);
	pthread_t consumers[num_consumers];
	pthread_t producer;
	printf("Creating producer...\n");
	int return_code = pthread_create(&producer,NULL,
									produce,(void *)buff);
	check_results("pthread_create() consumer\n", return_code);
	int i;
	for(i = 0; i < num_consumers; i++) {
		printf("Creating consumer: %d\n", i);
		return_code = pthread_create(&consumers[i],NULL,
									consume,(void *)buff);
		check_results("pthread_create() producer\n", return_code);
	}
	for(i = 0; i < num_consumers; i++) {
		pthread_join(consumers[i], NULL);
	}
	pthread_join(producer, NULL);
	return 0;
}