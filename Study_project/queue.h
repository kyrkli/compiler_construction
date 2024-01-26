#ifndef QUEUE_H
#define QUEUE_H
#include "stack.h"

//Define the structure for a queue node
typedef struct QueueNode {
	stackval_t data;
	struct QueueNode* next;
} QueueNode;

// Define the structure for the queue
typedef struct {
	QueueNode* front;
	QueueNode* rear;
} Queue;

// Function declarations
Queue* createQueue();
void enqueue(Queue* queue, stackval_t data);
stackval_t dequeue(Queue* queue);
int isEmpty(Queue* queue);
void clearQueue(Queue* queue);
void printQueue(Queue* queue);
#endif // QUEUE_H

