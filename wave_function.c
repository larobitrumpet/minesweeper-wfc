#include "wave_function.h"

void enqueue_neighbors(BOARD* board, POINT point) {
    bool left = point.x > 0;
    bool right = point.x < board->width - 1;
    bool up = point.y > 0;
    bool down = point.y < board->height - 1;
    bool mask[9] = {
        left && up,
        up,
        right && up,
        left,
        false,
        right,
        left && down,
        down,
        right && down
    };
    POINT p;
    queue_enqueue(&board->update_queue, &point);
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p.x = point.x + x;
                p.y = point.y + y;
                queue_enqueue(&board->update_queue, &p);
            }
        }
    }
}

int num_possibilities(uint_fast16_t tile) {
    int num = 0;
    for (int i = 0; i < 10; i++) {
        if (tile & 1) {
            num++;
        }
        tile >>= 1;
    }
    return num;
}

void num_mine_neighbors(BOARD* board, POINT point, unsigned int mines[2]) {
    bool left = point.x > 0;
    bool right = point.x < board->width - 1;
    bool up = point.y > 0;
    bool down = point.y < board->height - 1;
    bool mask[9] = {
        left && up,
        up,
        right && up,
        left,
        false,
        right,
        left && down,
        down,
        right && down
    };
    POINT p;
    mines[0] = 0;
    mines[1] = 0;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p.x = point.x + x;
                p.y = point.y + y;
                if ((get_tile_point(board, p) & 0b010000001111) == 0b010000001001) {
                    mines[0]++;
                    mines[1]++;
                } else if ((get_tile_point(board, p) & 0b011000000000) == 0b001000000000) {
                    mines[1]++;
                }
            }
        }
    }
}

void collapse_mine_neighbors(BOARD* board, POINT point) {
    bool left = point.x > 0;
    bool right = point.x < board->width - 1;
    bool up = point.y > 0;
    bool down = point.y < board->height - 1;
    bool mask[9] = {
        left && up,
        up,
        right && up,
        left,
        false,
        right,
        left && down,
        down,
        right && down
    };
    POINT p;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p.x = point.x + x;
                p.y = point.y + y;
                #ifdef DEBUG
                printf("Checking (%d, %d)\n", p.x, p.y);
                #endif
                if ((get_tile_point(board, p) & 0b011000000000) == 0b001000000000) {
                    #ifdef DEBUG
                    printf("Collapsing\n");
                    #endif
                    set_tile_point(board, p, 0b010000001001);
                    enqueue_neighbors(board, p);
                }
            }
        }
    }
}

void collapse_non_mine_neighbors(BOARD* board, POINT point) {
    bool left = point.x > 0;
    bool right = point.x < board->width - 1;
    bool up = point.y > 0;
    bool down = point.y < board->height - 1;
    bool mask[9] = {
        left && up,
        up,
        right && up,
        left,
        false,
        right,
        left && down,
        down,
        right && down
    };
    POINT p;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p.x = point.x + x;
                p.y = point.y + y;
                #ifdef DEBUG
                printf("Checking (%d, %d)\n", p.x, p.y);
                #endif
                if ((get_tile_point(board, p) & 0b011000000000) == 0b001000000000) {
                    #ifdef DEBUG
                    printf("Collapsing\n");
                    #endif
                    set_tile_point(board, p, get_tile_point(board, p) & 0b110111111111);
                    enqueue_neighbors(board, p);
                }
            }
        }
    }
}

void update_wave_function(BOARD* board) {
    #ifdef DEBUG
    printf("------------------------------------------------------------------------\n");
    printf("Updating\n");
    #endif
    while (!(queue_is_empty(&board->update_queue))) {
        POINT point;
        queue_dequeue(&board->update_queue, &point);
        uint_fast16_t tile = get_tile_point(board, point);
        #ifdef DEBUG
        printf("Point: (%d, %d)\n", point.x, point.y);
        printf("Tile: %" PRIxFAST16 "\n", tile);
        #endif
        if (tile & 0b010000000000) {
            #ifdef DEBUG
            printf("Tile is known\n");
            #endif
            unsigned int mines[2];
            num_mine_neighbors(board, point, mines);
            if (mines[0] == mines[1]) {
                #ifdef DEBUG
                printf("Mines already collapsed\n");
                #endif
                continue;
            }
            if (mines[0] == (tile & 0b1111)) {
                #ifdef DEBUG
                printf("Collapsing non mine neighbors\n");
                #endif
                collapse_non_mine_neighbors(board, point);
                continue;
            }
            if (mines[1] == (tile & 0b1111)) {
                #ifdef DEBUG
                printf("Collapsing mine neighbors\n");
                #endif
                collapse_mine_neighbors(board, point);
                continue;
            }
            continue;
        }
        int possibilities = num_possibilities(tile);
        assert(possibilities > 0);
        if (possibilities == 1) {
            int i;
            for (i = 0; i < 10; i++) {
                if (tile & 1 << i)
                    break;
            }
            #ifdef DEBUG
            printf("Only 1 possibility: %d\n", i);
            #endif
            tile = 0b010000000000 | i;
            set_tile_point(board, point, tile);
            enqueue_neighbors(board, point);
            continue;
        }
        unsigned int mines[2];
        num_mine_neighbors(board, point, mines);
        if (mines[0] == mines[1] && !(tile & 0b001000000000)) {
            #ifdef DEBUG
            printf("Is a number: %d\n", mines[0]);
            #endif
            tile = 0b010000000000 | mines[0];
            set_tile_point(board, point, tile);
            enqueue_neighbors(board, point);
            continue;
        }
        #ifdef DEBUG
        printf("Min mines: %d\n", mines[0]);
        printf("Max mines: %d\n", mines[1]);
        #endif
        uint_fast16_t old_tile = tile;
        for (unsigned int i = 0; i < mines[0]; i++) {
            #ifdef DEBUG
            printf("Removing %d\n", i);
            #endif
            tile &= ~(1 << i);
        }
        for (int i = mines[1] + 1; i < 9; i++) {
            #ifdef DEBUG
            printf("Removing %d\n", i);
            #endif
            tile &= ~(1 << i);
        }
        if (old_tile == tile) {
            #ifdef DEBUG
            printf("No change\n");
            #endif
            continue;
        }
        #ifdef DEBUG
        printf("Tile is now %" PRIxFAST16 "\n" , tile);
        #endif
        set_tile_point(board, point, tile);
        enqueue_neighbors(board, point);
    }
}
