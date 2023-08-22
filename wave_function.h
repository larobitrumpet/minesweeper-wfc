#ifndef WAVE_FUNCTION_H
#define WAVE_FUNCITON_H

#ifdef DEBUG
#include <stdio.h>
#include <inttypes.h>
#endif

#include <stdint.h>
#include <assert.h>
#include "queue.h"
#include "board.h"

void enqueue_neighbors(BOARD* board, POINT point);
int num_possibilities(uint_fast16_t tile);
void num_mine_neighbors(BOARD* board, POINT point, unsigned int mines[2]);
void collapse_mine_neighbors(BOARD* board, POINT point);
void collapse_non_mine_neighbors(BOARD* board, POINT point);
void update_wave_function(BOARD* board);

#endif
