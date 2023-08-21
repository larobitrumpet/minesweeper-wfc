#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "allegro5/allegro5.h"
#include "allegro5/allegro_image.h"

#define TILE_W 22
#define TILE_H 22

typedef struct SPRITES {
    ALLEGRO_BITMAP* _sheet;
    ALLEGRO_BITMAP* selected;
    ALLEGRO_BITMAP* unknown_base;
    ALLEGRO_BITMAP* unknown_base_no_mine;
    ALLEGRO_BITMAP* unknown[11];
    ALLEGRO_BITMAP* known[11];
} SPRITES;

ALLEGRO_BITMAP* sprite_grab(int x, int y, int w, int h);
void sprites_init();
void sprites_deinit();
void render_tile(uint_fast16_t tile, int x, int y);
void render(uint_fast16_t* tiles, int width, int height);

#endif
