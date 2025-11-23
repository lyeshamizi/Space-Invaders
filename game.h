#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <stdint.h>
#include "si.h"

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_GAME_OVER
} GameState;

typedef struct {
    SDL_Window *win;
    SDL_Renderer *ren;

    int window_width;
    int window_height;
    int pixel_size;
    GameState game_state;

    Si *si;

    uint64_t freq;
    uint64_t count_invaders;
    uint64_t count_shoot;

    char update;
} Game;

Game *game_new(void);
void game_del(Game *g);
void game_run(Game *g);

#endif