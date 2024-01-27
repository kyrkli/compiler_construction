#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

//Function to create an empty queue
Queue* createQueue() {
	Queue* queue = (Queue*)malloc(sizeof(Queue));
	if (queue == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	queue->front = queue->rear = NULL;
	return queue;
}

// Function to add an item to the queue
void enqueue(Queue* queue, stackval_t data) {
	QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
	if (newNode == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	newNode->data = data;
	newNode->next = NULL;

	if (isEmpty(queue)) {
	queue->front = queue->rear = newNode;
	} else {
	queue->rear->next = newNode;
	queue->rear = newNode;
	}
}

// Function to remove and return an item from the queue
stackval_t dequeue(Queue* queue) {
	if (isEmpty(queue)) {
		return (stackval_t) {.type = _error};
	}

	QueueNode* temp = queue->front;
	stackval_t data = temp->data;

	if (queue->front == queue->rear) {
		queue->front = queue->rear = NULL;
	} else {
		queue->front = queue->front->next;
	}

	free(temp);
	return data;
}

// Function to check if the queue is empty
int isEmpty(Queue* queue) {
	return queue->front == NULL;
}

void printQueue(Queue* queue) {
	if (isEmpty(queue)) {
		printf("Queue is empty\n");
		return;
	}

	QueueNode* current = queue->front;
	printf("Queue: ");
	while (current != NULL) {
		print_gdata(current->data);

		current = current->next;
	}
}

// Function to peek at the front element of the queue
stackval_t peek(Queue *queue) {
	if (isEmpty(queue)) {
		printf("Queue is empty.\n");
		exit(1);
	}
	return queue->front->data;
}

// Function to clear the queue
void clearQueue(Queue* queue) {
	while (!isEmpty(queue)) {
		dequeue(queue);
	}
	free(queue);
}
