#include "Queue.h"

Queue *CreateQueue(){
	Queue *q = (Queue*)malloc(sizeof(Queue));
	q->front = NULL;
	q->back = NULL;
	return q;
}

bool IsEmptyQueue (Queue *q){
	return q->front == NULL;
}

void QueuePush (Queue *q, int val){
	queue_element *p = (queue_element*)malloc(sizeof(queue_element));
	p->value = val;
	p->next = NULL;
	if (IsEmptyQueue(q)){
		q->front = p;
		q->back = p;
	}
	else{
		q->back->next = p;
		q->back = p;
	}
}

char QueuePop (Queue *q){
	queue_element *front = q->front;
	char v = front->value;
	q->front = front->next;
	front = NULL;
	free(front);
	return v;
}

void DestroyQueue (Queue *q){
	while (!IsEmptyQueue(q)){
		QueuePop(q);
	}
	free(q);
}

void PrintQueue (Queue *q){
    	queue_element *q1, *q2;
    	q1 = q -> front;
    	q2 = q1 -> next;
    	while (q1 != NULL){
        	printf("%d",q1->value);
		printf("\n");
        	q1 = q2;
        	if (q1 != NULL){
            		q2 = q1->next;
    		}	
	}
}

