#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "allegro5/allegro5.h"
#include "allegro5/allegro_image.h"
#include "render.h"
#include "wave_function.h"
#include "queue.h"

bool can_undo;
int selected[2];
int select_move_cooldown;

void must_init(bool test, const char *description)
{
    if (test) return;

    printf("Couldn't initialize %s\n", description);
}

#define BUFFER_W 352
#define BUFFER_H 352

float DISP_SCALE_W;
float DISP_SCALE_H;
float DISP_W;
float DISP_H;
float DISP_W_OFFSET;
float DISP_H_OFFSET;

ALLEGRO_DISPLAY *disp;
ALLEGRO_BITMAP *buffer;

void disp_init()
{
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW + ALLEGRO_RESIZABLE);
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);

    disp = al_create_display(BUFFER_W, BUFFER_H);
    must_init(disp ,"display");

    buffer = al_create_bitmap(BUFFER_W, BUFFER_H);
    must_init(buffer, "bitmap buffer");
}

void disp_deinit()
{
    al_destroy_bitmap(buffer);
    al_destroy_display(disp);
}

void disp_pre_draw()
{
    al_set_target_bitmap(buffer);
}

void disp_post_draw()
{
    float BUFFER_ASPECT = (float)BUFFER_W / (float)BUFFER_H;
    float D_W = (float)al_get_display_width(disp);
    float D_H = (float)al_get_display_height(disp);
    float DISP_ASPECT = D_W / D_H;

    DISP_SCALE_W = D_W / BUFFER_W;
    DISP_SCALE_H = D_H / BUFFER_H;
    
    if (DISP_ASPECT < BUFFER_ASPECT)
    {
        DISP_W = BUFFER_W * DISP_SCALE_W;
        DISP_H = BUFFER_H * DISP_SCALE_W;

        float DD_H = BUFFER_H * DISP_SCALE_H;

        DISP_H_OFFSET = (DD_H - DISP_H) / 2;
        DISP_W_OFFSET = 0;
    }
    else if (DISP_ASPECT > BUFFER_ASPECT)
    {
        DISP_W = BUFFER_W * DISP_SCALE_H;
        DISP_H = BUFFER_H * DISP_SCALE_H;

        float DD_W = BUFFER_W * DISP_SCALE_W;

        DISP_W_OFFSET = (DD_W - DISP_W) / 2;
        DISP_H_OFFSET = 0;
    }
    else
    {
        DISP_W = BUFFER_W * DISP_SCALE_W;
        DISP_H = BUFFER_H * DISP_SCALE_H;

        DISP_W_OFFSET = 0;
        DISP_H_OFFSET = 0;
    }

    al_set_target_backbuffer(disp);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_scaled_bitmap(buffer, 0, 0, BUFFER_W, BUFFER_H, DISP_W_OFFSET, DISP_H_OFFSET, DISP_W, DISP_H, 0);

    al_flip_display();
}

#define KEY_SEEN     1
#define KEY_RELEASED 2
unsigned char key[ALLEGRO_KEY_MAX];

void keyboard_init()
{
    memset(key, 0, sizeof(key));
}

void keyboard_update(ALLEGRO_EVENT* event)
{
    switch (event->type)
    {
        case ALLEGRO_EVENT_TIMER:
            for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
                key[i] &= KEY_SEEN;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            key[event->keyboard.keycode] = KEY_SEEN | KEY_RELEASED;
            break;
        case ALLEGRO_EVENT_KEY_UP:
            key[event->keyboard.keycode] &= KEY_RELEASED;
            break;
    }
}

void draw(uint_fast16_t* tiles, int board_width, int board_height)
{
    disp_pre_draw();

    // draw code
    render(tiles, board_width, board_height);

    disp_post_draw();
}

void parse_input(uint_fast16_t** tiles, uint_fast16_t** prev_tiles, QUEUE* update_queue, int board_width, int board_height) {
    if (!(key[ALLEGRO_KEY_LEFT] || key[ALLEGRO_KEY_RIGHT] || key[ALLEGRO_KEY_UP] || key[ALLEGRO_KEY_DOWN])) {
        select_move_cooldown = 0;
    }
    if (select_move_cooldown) {
        select_move_cooldown--;
    } else {
        (*tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
        if (key[ALLEGRO_KEY_LEFT]) {
            selected[0]--;
            select_move_cooldown = 8;
        }
        if (key[ALLEGRO_KEY_RIGHT]) {
            selected[0]++;
            select_move_cooldown = 8;
        }
        if (key[ALLEGRO_KEY_UP]) {
            selected[1]--;
            select_move_cooldown = 8;
        }
        if (key[ALLEGRO_KEY_DOWN]) {
            selected[1]++;
            select_move_cooldown = 8;
        }
        if (selected[0] < 0)
            selected[0] = 0;
        if (selected[0] > board_width - 1)
            selected[0] = board_width - 1;
        if (selected[1] < 0)
            selected[1] = 0;
        if (selected[1] > board_height - 1)
            selected[1] = board_height - 1;
        (*tiles)[selected[1] * board_height + selected[0]] |= 0b100000000000;
    }
    if (!((*tiles)[selected[1] * board_height + selected[0]] & 0b010000000000)) {
        if (key[ALLEGRO_KEY_0]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000000;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_1]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000001;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_2]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000010;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_3]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000011;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_4]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000100;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_5]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000101;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_6]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000110;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_7]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000000111;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_8]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            (*tiles)[selected[1] * board_height + selected[0]] = 0b110000001000;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
        if (key[ALLEGRO_KEY_M]) {
            memcpy(*prev_tiles, *tiles, board_width * board_height * sizeof(uint_fast16_t));
            (*prev_tiles)[selected[1] * board_height + selected[0]] &= 0b011111111111;
            can_undo = true;
            *tiles[selected[1] * board_height + selected[0]] = 0b110000001001;
            enqueue_neighbors(update_queue, selected, board_width, board_height);
            update_wave_function(*tiles, update_queue, board_width, board_height);
        }
    }
    if (key[ALLEGRO_KEY_U] && can_undo) {
        uint_fast16_t* tmp = *tiles;
        *tiles = *prev_tiles;
        *prev_tiles = tmp;
        can_undo = false;
    }
}

int main()
{
    int board_width = 16;
    int board_height = 16;
    uint_fast16_t* tiles = (uint_fast16_t*)malloc(sizeof(uint_fast16_t) * board_width * board_height);
    uint_fast16_t* prev_tiles = (uint_fast16_t*)malloc(sizeof(uint_fast16_t) * board_width * board_height);
    can_undo = false;
    QUEUE update_queue = construct_queue(sizeof(int) * 2);
    for (int y = 0; y < board_height; y++) {
        for (int x = 0; x < board_width; x++) {
            tiles[y * board_height + x] = 0b001111111111;
            int p[2] = {x, y};
            queue_enqueue(&update_queue, p);
        }
    }
    tiles[selected[1] * board_height + selected[0]] |= 0b100000000000;
    update_wave_function(tiles, &update_queue, board_width, board_height);

    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;

    int redraw = true;
    must_init(al_init(), "Allegro");
    must_init(al_init_image_addon(), "image addon");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");

    // init
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);
    al_set_new_display_flags(ALLEGRO_RESIZABLE);

    disp_init();

    keyboard_init();

    sprites_init();

    timer = al_create_timer(1.0 / 60);
    must_init(timer, "timer");

    queue = al_create_event_queue();
    must_init(queue, "queue");
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // setup_scene();

    bool done = false;
    al_start_timer(timer);
    while(true)
    {
        ALLEGRO_EVENT event;

        al_wait_for_event(queue, &event);
        switch (event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // game logic
                parse_input(&tiles, &prev_tiles, &update_queue, board_width, board_height);
                redraw = true;
                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
            case ALLEGRO_EVENT_DISPLAY_RESIZE:
                al_acknowledge_resize(disp);
                break;
            case ALLEGRO_EVENT_KEY_DOWN:
                switch (event.keyboard.keycode)
                {
                    case ALLEGRO_KEY_ESCAPE:
                        done = true;
                        break;
                }
                break;
        }

        if (done)
        {
            break;
        }

        keyboard_update(&event);

        if (redraw && al_is_event_queue_empty(queue))
        {
            // redraw code
            draw(tiles, board_width, board_height);
            redraw = false;
        }
    }

    // deinit
    deconstruct_queue(update_queue);
    free(tiles);
    free(prev_tiles);
    sprites_deinit();
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
