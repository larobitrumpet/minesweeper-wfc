#include "board.h"

BOARD construct_board(int width, int height) {
    BOARD board;
    board.width = width;
    board.height = height;
    board.num_tiles = board.width * board.height;
    board.size = board.num_tiles * sizeof(uint_fast16_t);
    board.selected.x = 0;
    board.selected.y = 0;
    board.tiles = (uint_fast16_t*)malloc(board.size);
    board.prev_tiles = (uint_fast16_t*)malloc(board.size);
    board.select_move_cooldown = 0;
    board.can_undo = false;
    board.update_queue = construct_queue(sizeof(POINT));
    for (int y = 0; y < board.height; y++) {
        for (int x = 0; x < board.width; x++) {
            board.tiles[y * board.height + x] = 0b001111111111;
            POINT p;
            p.x = x;
            p.y = y;
            queue_enqueue(&board.update_queue, &p);
        }
    }
    board.tiles[board.selected.y * board.height + board.selected.x] |= 0b100000000000;
    update_wave_function(&board);
    return board;
}

void deconstruct_board(BOARD board) {
    free(board.tiles);
    free(board.prev_tiles);
    deconstruct_queue(board.update_queue);
}

int point_to_index(POINT p, int height) {
    return p.y * height + p.x;
}

int board_index(BOARD* board) {
    return point_to_index(board->selected, board->height);
}

uint_fast16_t get_tile_point(BOARD* board, POINT p) {
    return board->tiles[point_to_index(p, board->height)];
}

uint_fast16_t get_tile(BOARD* board) {
    return board->tiles[board_index(board)];
}

void set_tile_point(BOARD* board, POINT p, uint_fast16_t value) {
    board->tiles[point_to_index(p, board->height)] = value;
}

void set_tile(BOARD* board, uint_fast16_t value) {
    board->tiles[board_index(board)] = value;
}

void prev_tiles_remove_selected(BOARD* board) {
    board->prev_tiles[board_index(board)] &= 0b011111111111;
}

void move_left(BOARD* board) {
    board->selected.x--;
    board->select_move_cooldown = SELECT_MOVE_COOLDOWN_TIME;
    if (board->selected.x < 0)
        board->selected.x = 0;
}

void move_right(BOARD* board) {
    board->selected.x++;
    board->select_move_cooldown = SELECT_MOVE_COOLDOWN_TIME;
    if (board->selected.x > board->width - 1)
        board->selected.x = board->width - 1;
}

void move_up(BOARD* board) {
    board->selected.y--;
    board->select_move_cooldown = SELECT_MOVE_COOLDOWN_TIME;
    if (board->selected.y < 0)
        board->selected.y = 0;
}

void move_down(BOARD* board) {
    board->selected.y++;
    board->select_move_cooldown = SELECT_MOVE_COOLDOWN_TIME;
    if (board->selected.y > board->width - 1)
        board->selected.y = board->width - 1;
}

void set_selected(BOARD* board) {
    set_tile(board, get_tile(board) | 0b100000000000);
}

void clear_selected(BOARD* board) {
    set_tile(board, get_tile(board) & 0b011111111111);
}

bool is_known(BOARD* board) {
    return get_tile(board) & 0b010000000000;
}

void make_known(BOARD* board, uint_fast16_t value) {
    memcpy(board->prev_tiles, board->tiles, board->size);
    prev_tiles_remove_selected(board);
    board->can_undo = true;
    set_tile(board, 0b110000000000 | value);
    enqueue_neighbors(board, board->selected);
    update_wave_function(board);
}

void undo(BOARD* board) {
    if (!board->can_undo)
        return;
    uint_fast16_t* tmp = board->tiles;
    board->tiles = board->prev_tiles;
    board->prev_tiles = tmp;
    board->can_undo = false;
}
