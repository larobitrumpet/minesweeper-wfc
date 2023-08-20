#include "wave_function.h"

void enqueue_neighbors(QUEUE* update_queue, int point[2], int board_width, int board_height) {
    bool left = point[0] > 0;
    bool right = point[0] < board_width - 1;
    bool up = point[1] > 0;
    bool down = point[1] < board_height - 1;
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
    int p[2];
    queue_enqueue(update_queue, point);
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p[0] = point[0] + x;
                p[1] = point[1] + y;
                queue_enqueue(update_queue, p);
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

void num_mine_neighbors(uint_fast16_t* tiles, int point[2], unsigned int mines[2], int board_width, int board_height) {
    bool left = point[0] > 0;
    bool right = point[0] < board_width - 1;
    bool up = point[1] > 0;
    bool down = point[1] < board_height - 1;
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
    int p[2];
    mines[0] = 0;
    mines[1] = 0;
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p[0] = point[0] + x;
                p[1] = point[1] + y;
                if ((tiles[p[1] * board_height + p[0]] & 0b010000001111) == 0b010000001001) {
                    mines[0]++;
                    mines[1]++;
                } else if ((tiles[p[1] * board_height + p[0]] & 0b011000000000) == 0b001000000000) {
                    mines[1]++;
                }
            }
        }
    }
}

void collapse_mine_neighbors(uint_fast16_t* tiles, QUEUE* update_queue, int point[2], int board_width, int board_height) {
    bool left = point[0] > 0;
    bool right = point[0] < board_width - 1;
    bool up = point[1] > 0;
    bool down = point[1] < board_height - 1;
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
    int p[2];
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p[0] = point[0] + x;
                p[1] = point[1] + y;
                #ifdef DEBUG
                printf("Checking (%d, %d)\n", p[0], p[1]);
                #endif
                if ((tiles[p[1] * board_height + p[0]] & 0b011000000000) == 0b001000000000) {
                    #ifdef DEBUG
                    printf("Collapsing\n");
                    #endif
                    tiles[p[1] * board_height + p[0]] = 0b010000001001;
                    enqueue_neighbors(update_queue, p, board_width, board_height);
                }
            }
        }
    }
}

void collapse_non_mine_neighbors(uint_fast16_t* tiles, QUEUE* update_queue, int point[2], int board_width, int board_height) {
    bool left = point[0] > 0;
    bool right = point[0] < board_width - 1;
    bool up = point[1] > 0;
    bool down = point[1] < board_height - 1;
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
    int p[2];
    for (int y = -1; y < 2; y++) {
        for (int x = -1; x < 2; x++) {
            if (mask[(y + 1) * 3 + (x + 1)]) {
                p[0] = point[0] + x;
                p[1] = point[1] + y;
                #ifdef DEBUG
                printf("Checking (%d, %d)\n", p[0], p[1]);
                #endif
                if ((tiles[p[1] * board_height + p[0]] & 0b011000000000) == 0b001000000000) {
                    #ifdef DEBUG
                    printf("Collapsing\n");
                    #endif
                    tiles[p[1] * board_height + p[0]] &= 0b110111111111;
                    enqueue_neighbors(update_queue, p, board_width, board_height);
                }
            }
        }
    }
}

void update_wave_function(uint_fast16_t* tiles, QUEUE* update_queue, int board_width, int board_height) {
    #ifdef DEBUG
    printf("------------------------------------------------------------------------\n");
    printf("Updating\n");
    #endif
    while (!(queue_is_empty(update_queue))) {
        int point[2];
        queue_dequeue(update_queue, point);
        uint_fast16_t tile = tiles[point[1] * board_height + point[0]];
        #ifdef DEBUG
        printf("Point: (%d, %d)\n", point[0], point[1]);
        printf("Tile: %lx\n", tile);
        #endif
        if (tile & 0b010000000000) {
            #ifdef DEBUG
            printf("Tile is known\n");
            #endif
            unsigned int mines[2];
            num_mine_neighbors(tiles, point, mines, board_width, board_height);
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
                collapse_non_mine_neighbors(tiles, update_queue, point, board_width, board_height);
                continue;
            }
            if (mines[1] == (tile & 0b1111)) {
                #ifdef DEBUG
                printf("Collapsing mine neighbors\n");
                #endif
                collapse_mine_neighbors(tiles, update_queue, point, board_width, board_height);
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
            tiles[point[1] * board_height + point[0]] = tile;
            enqueue_neighbors(update_queue, point, board_width, board_height);
            continue;
        }
        unsigned int mines[2];
        num_mine_neighbors(tiles, point, mines, board_width, board_height);
        if (mines[0] == mines[1] && !(tile & 0b001000000000)) {
            #ifdef DEBUG
            printf("Is a number: %d\n", mines[0]);
            #endif
            tile = 0b010000000000 | mines[0];
            tiles[point[1] * board_height + point[0]] = tile;
            enqueue_neighbors(update_queue, point, board_width, board_height);
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
        printf("Tile is now %lx\n", tile);
        #endif
        tiles[point[1] * board_height + point[0]] = tile;
        enqueue_neighbors(update_queue, point, board_width, board_height);
    }
}
