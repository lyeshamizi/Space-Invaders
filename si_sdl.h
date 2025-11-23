#ifndef SI_SDL_H
#define SI_SDL_H

#include "game.h"

void si_display_sprite(Game *g, char *sprite, int w, int x, int y);
void si_tank_display(Game *g, int x, int y);
void si_tank_explode_display(Game *g, int type, int x, int y);
void si_tank_shoot_display(Game *g, int x, int y);
void si_tank_set_position(Game *g);
void si_ufo_display(Game *g, int x, int y);
void si_invader_display(Game *g, int type, int model, int x, int y);
void si_invader_explode_display(Game *g, int x, int y);
void si_invader_shoot_display(Game *g, int x, int y);
void si_invaders_display(Game *g, int x, int y);
void si_alphanum_display(Game *g, char c, int x, int y);
void si_str_display(Game *g, const char *str, int x, int y);
void si_text_display(Game *g, const char *str, int l, int c);
void si_explosion_display(Game *g, int x, int y, int type, int frame);

#endif