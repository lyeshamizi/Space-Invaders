#include "si_sdl.h"
#include "si_font.h"
#include <SDL2/SDL.h>
#include <string.h>

void si_display_sprite(Game *g, char *sprite, int w, int x, int y)
{
    if (!g || !sprite) return;

    int pixel_size = g->pixel_size;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < w; j++) {
            if (sprite[i * w + j]) {
                SDL_Rect rect = {
                    x + j * pixel_size,
                    y + i * pixel_size,
                    pixel_size,
                    pixel_size
                };
                SDL_SetRenderDrawColor(g->ren, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(g->ren, &rect);
            }
        }
    }
}

void si_tank_display(Game *g, int x, int y)
{
    int width;
    char *sprite = si_font_tank_get(&width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_tank_explode_display(Game *g, int type, int x, int y)
{
    int width;
    char *sprite = si_font_tank_explode_get(type, &width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_tank_shoot_display(Game *g, int x, int y)
{
    int width;
    char *sprite = si_font_tank_shoot_get(&width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_tank_set_position(Game *g)
{
    if (!g || !g->si) return;
    // Cette fonction peut être utilisée pour positionner automatiquement le tank
}

void si_ufo_display(Game *g, int x, int y)
{
    int width;
    char *sprite = si_font_ufo_get(&width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_invader_display(Game *g, int type, int model, int x, int y)
{
    int width;
    char *sprite = si_font_invader_get(type, model, &width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_invader_explode_display(Game *g, int x, int y)
{
    int width;
    char *sprite = si_font_invader_explode_get(&width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_invader_shoot_display(Game *g, int x, int y)
{
    int width;
    char *sprite = si_font_invader_shoot_get(&width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_invaders_display(Game *g, int x, int y)
{
    // Cette fonction est maintenant intégrée dans le rendu principal de game.c
    // On la laisse pour compatibilité
    if (!g || !g->si) return;

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 11; col++) {
            if (g->si->invaders.matrix[row][col] != 0) {
                Si_Type type = (Si_Type)g->si->invaders.matrix[row][col];

                int invader_width = 12 * g->pixel_size;
                int col_spacing = 2 * g->pixel_size;
                int row_spacing = 10 * g->pixel_size;

                int pos_x = x + col * (invader_width + col_spacing);
                int pos_y = y + row * row_spacing;

                si_invader_display(g, type, g->si->invaders.current_frame, pos_x, pos_y);
            }
        }
    }
}

void si_alphanum_display(Game *g, char c, int x, int y)
{
    int width;
    char *sprite = si_font_alphanum_get(c, &width);
    si_display_sprite(g, sprite, width, x, y);
}

void si_str_display(Game *g, const char *str, int x, int y)
{
    if (!str) return;

    int current_x = x;
    for (int i = 0; str[i] != '\0'; i++) {
        si_alphanum_display(g, str[i], current_x, y);
        current_x += 6 * g->pixel_size; // 5 pixels + 1 espace
    }
}

void si_text_display(Game *g, const char *str, int l, int c)
{
    int x = c * 6 * g->pixel_size + 2 * g->pixel_size; // Léger décalage
    int y = l * 8 * g->pixel_size + 2 * g->pixel_size; // Léger décalage
    si_str_display(g, str, x, y);
}

void si_explosion_display(Game *g, int x, int y, int type, int frame)
{
    if (type == 0) {
        // Explosion d'envahisseur
        si_invader_explode_display(g, x - 6 * g->pixel_size, y - 4 * g->pixel_size);
    } else {
        // Explosion du tank
        si_tank_explode_display(g, frame, x - 8 * g->pixel_size, y - 4 * g->pixel_size);
    }
}