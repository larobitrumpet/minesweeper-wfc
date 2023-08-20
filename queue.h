#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define QUEUE_INIT_SIZE 16
#define QUEUE_SIZE_INCREASE 1.5

typedef struct QUEUE {
    void* q;
    size_t bytewidth;
    int size;
    int head;
    int tail;
} QUEUE;

QUEUE construct_queue(size_t bytewidth);
void deconstruct_queue(QUEUE queue);
bool queue_is_empty(QUEUE* queue);
int queue_length(QUEUE* queue);
void queue_increase_size(QUEUE* queue);
void queue_enqueue(QUEUE* queue, void* data);
int queue_dequeue(QUEUE* queue, void* data);

#endif
