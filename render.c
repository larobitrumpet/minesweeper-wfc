#include "render.h"

SPRITES sprites;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h) {
    ALLEGRO_BITMAP* sprite = al_create_sub_bitmap(sprites._sheet, x, y, w, h);
    //must_init(sprite, "sprite grab");
    return sprite;
}

void sprites_init() {
    sprites._sheet = al_load_bitmap("minesweeper-wfc.png");
    //must_init(sprites._sheet, "sprite sheet");
    sprites.selected = sprite_grab(10 * TILE_W, TILE_H, TILE_W, TILE_H);
    sprites.unknown_base = sprite_grab(10 * TILE_W, 0, TILE_W, TILE_H);
    sprites.unknown_base_no_mine = sprite_grab(11 * TILE_W, 0, TILE_W, TILE_H);
    for (int i = 0; i < 10; i++) {
        sprites.unknown[i] = sprite_grab(i * TILE_W, TILE_H, TILE_W, TILE_H);
        sprites.known[i] = sprite_grab(i * TILE_W, 0, TILE_W, TILE_H);
    }
}

void sprites_deinit() {
    for (int i = 0; i < 10; i++) {
        al_destroy_bitmap(sprites.known[i]);
        al_destroy_bitmap(sprites.unknown[i]);
    }
    al_destroy_bitmap(sprites.unknown_base_no_mine);
    al_destroy_bitmap(sprites.unknown_base);
    al_destroy_bitmap(sprites.selected);
    al_destroy_bitmap(sprites._sheet);
}

void render_tile(uint_fast16_t tile, int x, int y) {
    int dx = x * TILE_W;
    int dy = y * TILE_H;
    if (tile & 0b010000000000) {
        al_draw_bitmap(sprites.known[tile & 0b1111], dx, dy, 0);
    } else {
        if (tile & 0b001000000000)
            al_draw_bitmap(sprites.unknown_base, dx, dy, 0);
        else
            al_draw_bitmap(sprites.unknown_base_no_mine, dx, dy, 0);
        uint_fast16_t t = tile;
        for (int i = 0; i < 10; i++) {
            if (t & 1) {
                al_draw_bitmap(sprites.unknown[i], dx, dy, 0);
            }
            t >>= 1;
        }
    }
    if (tile & 0b100000000000) {
        al_draw_bitmap(sprites.selected, dx, dy, 0);
    }
}

void render(BOARD* board) {
    POINT p;
    for (int y = 0; y < board->height; y++) {
        for (int x = 0; x < board->width; x++) {
            p.x = x;
            p.y = y;
            render_tile(get_tile_point(board, p), x, y);
        }
    }
}
