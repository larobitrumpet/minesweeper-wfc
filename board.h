#ifndef BOARD_H
#define BOARD_H

#include <stdlib.h>
#include <stdint.h>
#include "queue.h"

#define SELECT_MOVE_COOLDOWN_TIME 8

typedef struct POINT {
    int x;
    int y;
} POINT;

typedef struct BOARD {
    uint_fast16_t* tiles;
    uint_fast16_t* prev_tiles;
    int width;
    int height;
    int num_tiles;
    size_t size;
    POINT selected;
    int select_move_cooldown;
    bool can_undo;
    QUEUE update_queue;
} BOARD;

#include "wave_function.h"

BOARD construct_board(int width, int height);
void deconstruct_board(BOARD board);
int point_to_index(POINT p, int height);
int board_index(BOARD* board);
uint_fast16_t get_tile_point(BOARD* board, POINT p);
uint_fast16_t get_tile(BOARD* board);
void set_tile_point(BOARD* board, POINT p, uint_fast16_t value);
void set_tile(BOARD* board, uint_fast16_t value);
void prev_tiles_remove_selected(BOARD* board);
void move_left(BOARD* board);
void move_right(BOARD* board);
void move_up(BOARD* board);
void move_down(BOARD* board);
void set_selected(BOARD* board);
void clear_selected(BOARD* board);
bool is_known(BOARD* board);
void make_known(BOARD* board, uint_fast16_t value);
void undo(BOARD* board);

#endif
