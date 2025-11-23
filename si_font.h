#ifndef SI_FONT_H
#define SI_FONT_H
#include "si.h"

char *si_font_alphanum_get(char c, int *width);
char *si_font_tank_get(int *width);
char *si_font_tank_explode_get(int type, int *width);
char *si_font_tank_shoot_get(int *width);
char *si_font_ufo_get(int *width);
char *si_font_invader_get(Si_Type t, int model, int *width);
char *si_font_invader_explode_get(int *width);
char *si_font_invader_shoot_get(int *width);

#endif