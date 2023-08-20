#ifndef WAVE_FUNCTION_H
#define WAVE_FUNCITON_H

#ifdef DEBUG
#include <stdio.h>
#endif

#include <stdint.h>
#include <assert.h>
#include "queue.h"

void enqueue_neighbors(QUEUE* update_queue, int point[2], int board_width, int board_height);
int num_possibilities(uint_fast16_t tile);
void num_mine_neighbors(uint_fast16_t* tiles, int point[2], uint mines[2], int board_width, int board_height);
void collapse_mine_neighbors(uint_fast16_t* tiles, QUEUE* update_queue, int point[2], int board_width, int board_height);
void collapse_non_mine_neighbors(uint_fast16_t* tiles, QUEUE* update_queue, int point[2], int board_width, int board_height);
void update_wave_function(uint_fast16_t* tiles, QUEUE* update_queue, int board_width, int board_height);

#endif
