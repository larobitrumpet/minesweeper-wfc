#include "queue.h"

static inline int mod(int a, int b) {
    return a < 0 ? a % b + b : a % b;
}

QUEUE construct_queue(size_t bytewidth) {
    QUEUE queue;
    queue.bytewidth = bytewidth;
    queue.size = QUEUE_INIT_SIZE;
    queue.q = malloc(queue.bytewidth * queue.size);
    queue.head = 0;
    queue.tail = 0;
    return queue;
}

void deconstruct_queue(QUEUE queue) {
    free(queue.q);
}

bool queue_is_empty(QUEUE* queue) {
    return queue->head == queue->tail;
}

int queue_length(QUEUE* queue) {
    return mod(queue->tail - queue->head, queue->size);
}

void queue_increase_size(QUEUE* queue) {
    int old_size = queue->size;
    queue->size = (int)(old_size * QUEUE_SIZE_INCREASE);
    void* new_q = malloc(queue->bytewidth * queue->size);
    int i = queue->head;
    int j = 0;
    while (i != queue->tail) {
        memcpy(new_q + j * queue->bytewidth, queue->q + i * queue->bytewidth, queue->bytewidth);
        i = mod(i + 1, old_size);
        j++;
    }
    free(queue->q);
    queue->q = new_q;
    queue->head = 0;
    queue->tail = j;
}

void queue_enqueue(QUEUE* queue, void* data) {
    if (queue->head == mod(queue->tail + 1, queue->size))
        queue_increase_size(queue);
    memcpy(queue->q + queue->tail * queue->bytewidth, data, queue->bytewidth);
    queue->tail = mod(queue->tail + 1, queue->size);
}

int queue_dequeue(QUEUE* queue, void* data) {
    if (queue_is_empty(queue)) {
        return 1;
    }
    memcpy(data, queue->q + queue->head * queue->bytewidth, queue->bytewidth);
    queue->head = mod(queue->head + 1, queue->size);
    return 0;
}
