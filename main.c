#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "allegro5/allegro5.h"
#include "allegro5/allegro_image.h"
#include "board.h"
#include "render.h"
#include "wave_function.h"
#include "queue.h"

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

void draw(BOARD* board)
{
    disp_pre_draw();

    // draw code
    render(board);

    disp_post_draw();
}

void parse_input(BOARD* board) {
    if (!(key[ALLEGRO_KEY_LEFT] || key[ALLEGRO_KEY_RIGHT] || key[ALLEGRO_KEY_UP] || key[ALLEGRO_KEY_DOWN])) {
        board->select_move_cooldown = 0;
    }
    if (board->select_move_cooldown) {
        board->select_move_cooldown--;
    } else {
        clear_selected(board);
        if (key[ALLEGRO_KEY_LEFT])
            move_left(board);
        if (key[ALLEGRO_KEY_RIGHT])
            move_right(board);
        if (key[ALLEGRO_KEY_UP])
            move_up(board);
        if (key[ALLEGRO_KEY_DOWN])
            move_down(board);
        set_selected(board);
    }
    if (!is_known(board)) {
        if (key[ALLEGRO_KEY_0])
            make_known(board, 0);
        if (key[ALLEGRO_KEY_1])
            make_known(board, 1);
        if (key[ALLEGRO_KEY_2])
            make_known(board, 2);
        if (key[ALLEGRO_KEY_3])
            make_known(board, 3);
        if (key[ALLEGRO_KEY_4])
            make_known(board, 4);
        if (key[ALLEGRO_KEY_5])
            make_known(board, 5);
        if (key[ALLEGRO_KEY_6])
            make_known(board, 6);
        if (key[ALLEGRO_KEY_7])
            make_known(board, 7);
        if (key[ALLEGRO_KEY_8])
            make_known(board, 8);
        if (key[ALLEGRO_KEY_M])
            make_known(board, 9);
    }
    if (key[ALLEGRO_KEY_U]) {
        undo(board);
    }
}

int main()
{
    BOARD board = construct_board(16, 16);

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
                parse_input(&board);
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
            draw(&board);
            redraw = false;
        }
    }

    // deinit
    deconstruct_board(board);
    sprites_deinit();
    disp_deinit();
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
