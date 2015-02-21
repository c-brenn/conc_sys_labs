#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[37m"

#define ITEMS_PER_CON 10
#define BUFFER_SIZE 40
#define ROW_SIZE 10
#define MAX_CONSUMERS 100
#define PRODUCE_TIME 100000000
#define CONSUME_TIME 500000000

typedef enum{
	FALSE, TRUE
}boolean;

typedef struct{
	char * data;
}item;

typedef struct{
	int size;
	int n;
	item ** items;
}buffer;

item * item_new(char * d);
void item_free(item * this);
void item_print(item * this);
void item_update_data(item * this, char * s);
buffer * buffer_new(int size);
void buffer_free(buffer * this);
void buffer_add_item(buffer * this, char * s);
void buffer_remove_item(buffer * this);
void buffer_print(buffer * this);
boolean buffer_is_full(buffer * this);
boolean buffer_is_empty(buffer * this);
void timer(long i);
void * produce(void* args);
void * consume(void * args);