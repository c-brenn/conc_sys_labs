#include "prod_con.h"

pthread_mutex_t buffer_mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty_cond = PTHREAD_COND_INITIALIZER;

int items_to_create;
int num_consumers;
int items_left;
boolean finished_producing = FALSE;

item * item_new(char * d) {
	item * this = malloc(sizeof(item));
	this->data = malloc(strlen(d) + 1);
	strcpy(this->data, d);
	return this;
}

void item_free(item * this) {
	free(this->data);
	free(this);
}

void item_print(item * this) {
	printf(RED "[%s]", this->data);
	return;
}

void item_update_data(item * this, char * s) {
	this->data = realloc(this->data, strlen(s) + 1);
	strcpy(this->data, s);
	return;
}

buffer * buffer_new(int size) {
	buffer * this = malloc(sizeof(buffer));
	this->size = size;
	this->n = 0;
	this->items = calloc(size, sizeof(item*));
	return this;
}

void buffer_free (buffer * this) {
	int i;
	for(i = 0; i < this->size; i++) {
		item_free(this->items[i]);
	}
	free(this->items);
	free(this);
	return;
}

void buffer_add_item (buffer * this, char * s) {
	if(this->items[this->n] == NULL) {
		this->items[this->n++] = item_new(s);
	} else {
		item_update_data(this->items[this->n++], s);
	}
	return;
}

void buffer_remove_item(buffer * this) {
	this->n--;
}

void buffer_print(buffer * this) {
	system("clear");
	printf("\n\tBuffer:\n\t");
	int i;
	for (i = 0; i < this->size; i++) {
		
		if (i % ROW_SIZE == 0) {
			printf(RESET "\n\t");
		}
		if (i < this->n) {
			item_print(this->items[i]);
		} else {
			printf(GREEN "[    ]");
		}
	}
	printf(RESET "\n\n\tItems in buffer: %d\n", this->n);
	printf(RESET "\tItems left to produce: %d\n", items_left);
	fflush(stdout);
	return;
}

boolean buffer_is_full(buffer * this) {
	return this->n == this->size;
}

boolean buffer_is_empty(buffer * this) {
	return this->n == 0;
}

void timer(long i) {
	int t = i;
	float r = (float) rand() / (float) RAND_MAX;
	t = (int) ((float) t * r);
	while(t > 0) {
		t--;
	}
	return;
}

void * produce(void * args) {
	buffer * buff = (buffer*) args;
	items_left = items_to_create;
	while(items_left > 0) {
		// simulate creating item
		char* data = "item";
		timer(PRODUCE_TIME);

		pthread_mutex_lock(&buffer_mut);
		if (!buffer_is_full(buff)) {
			// add an item to the buffer
			buffer_add_item(buff, data);
			buffer_print(buff);
			items_left--;
			// release the mutex and wake tell consumers that
			// the buffer isn't empty
			pthread_mutex_unlock(&buffer_mut);
			pthread_cond_broadcast(&empty_cond);
		} else {
			// wait for empty slot in buffer
			pthread_mutex_unlock(&buffer_mut);
			pthread_cond_wait(&full_cond, &buffer_mut);
		}
	}
	pthread_mutex_unlock(&buffer_mut);
	finished_producing = TRUE;
	pthread_exit(0);
}

void * consume(void * args) {
	buffer * buff = (buffer*) args;
	
	while(!finished_producing || !buffer_is_empty(buff)) {
		
		pthread_mutex_lock(&buffer_mut);
		if(!buffer_is_empty(buff)) {
			// remove and item from the buffer
			buffer_remove_item(buff);
			buffer_print(buff);
			// release the mutex and tell the producer
			// that the buffer isn't full
			pthread_mutex_unlock(&buffer_mut);
			pthread_cond_broadcast(&full_cond);
			timer(CONSUME_TIME);
		} else {
			// wait for producer to place an item in the buffer
			pthread_mutex_unlock(&buffer_mut);
			pthread_cond_wait(&empty_cond, &buffer_mut);
		}
		pthread_mutex_unlock(&buffer_mut);
	}
	pthread_mutex_unlock(&buffer_mut);
	pthread_exit(0);
}

int main(int argc, char ** argv) {
	// ensure that program was called with correct num of params
	if (argc < 2) {
		printf("I need args: <number of consumers>\n");
		exit(1);
	}
	// check that params are sensible
	num_consumers = atoi(argv[1]);
	if (num_consumers < 1 || num_consumers > MAX_CONSUMERS) {
		printf("That's a silly number of consumers\n");
		exit(1);
	}
	// create the buffer
	items_to_create = num_consumers * ITEMS_PER_CON;
	buffer * b = buffer_new(BUFFER_SIZE);
	
	// create and start producer
	pthread_t producer;
	if (pthread_create(&producer, NULL, produce, (void*) b)) {
		printf("Error creating producer thread.\n");
		exit(1);
	}
	pthread_cond_signal(&full_cond);

	//create and start consumers
	pthread_t consumers[num_consumers];
	int i;
	for (i = 0; i < num_consumers; i++) {
		printf("Created consumer: %d.\n", i);
		if (pthread_create(&consumers[i], NULL, consume, (void*) b)) {
			printf("Error creating consumer thread: %d.\n", i);
			exit(1);
		}
	}

	// wait for producer to join
	if (pthread_join(producer, NULL)){
		printf("Error joining producer thread.\n");
		exit(1);
	}

	// wait for consumers to join;
	for (i = 0; i < num_consumers; i++) {
		if (pthread_join(consumers[i], NULL)){
		printf("Error joining consumer thread: %d.\n", i);
		exit(1);
	}
	}

	return 0;
}
