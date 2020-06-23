#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct queue_element{
	int value;
	struct queue_element *next;
} queue_element;


typedef struct{
	queue_element *front;
	queue_element *back;
} Queue;


Queue *CreateQueue();

void DestroyQueue (Queue *q);

bool IsEmptyQueue (Queue *q);
	
void QueuePush (Queue *q, int val);

char QueuePop (Queue *q);

void PrintQueue (Queue *q);

	

