#include "ringQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** array;
int front;
int back;

void ringQueue_init(){
	array = (char**) malloc (RINGQ_SIZE * sizeof(char*));
	for(int i = 0; i < RINGQ_SIZE; i++)
		array[i] = (char*) malloc (30 * sizeof(char));

	front = -1;
	back = 0;
}

void ringQueue_deinit(){	
	for(int i = 0; i < RINGQ_SIZE; i++)
		free(array[i]);
	free(array);
}

int is_empty(){
	if ((front == -1 && back == 0) || front == back)
		return 1;
	else return 0;
}

void enqueue(char* el){
	if(front == -1) //if it is the first element
		front++;

	if(back == RINGQ_SIZE) //if it the queue had more than ringq_size elements
		back = 0;
	
	strcpy(array[back++], el);
	
}

char* dequeue() {
	if(is_empty())
		return NULL;
	else {
		if(front+1 == RINGQ_SIZE){
			front = 0;
			return array[RINGQ_SIZE - 1];
		}
		return array[front++];
	}

}

void print_queue(){
	int old_front = front;
	while(!is_empty())
		printf("Queue element №%d = %s\n", front, array[front++]);
	front = old_front;
	
}
