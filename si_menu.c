#include "si_menu.h"
#include "si_sdl.h"
#include <SDL2/SDL.h>

void menu(Game *g)
{
    SDL_SetRenderDrawColor(g->ren, 0x00, 0x00, 0x20, 0xFF);
    SDL_RenderClear(g->ren);

    // Ligne de scores en haut
    si_text_display(g, "SCORE<1>", 0, 1);
    si_text_display(g, "HI-SCORE", 0, 10);
    si_text_display(g, "SCORE<2>", 0, 19);

    // Valeurs des scores
    char score1_text[20], hi_score_text[20], score2_text[20];
    snprintf(score1_text, sizeof(score1_text), "%04d", g->si->score_1);
    snprintf(hi_score_text, sizeof(hi_score_text), "%04d", g->si->score_highest);
    snprintf(score2_text, sizeof(score2_text), "%04d", 0); // Score 2 à 0

    si_text_display(g, score1_text, 1, 1);
    si_text_display(g, hi_score_text, 1, 10);
    si_text_display(g, score2_text, 1, 19);

    // Titre principal
    si_text_display(g, "SPACE INVADERS", 5, 6);

    // Table des scores
    si_text_display(g, "*SCORE ADVANCE TABLE*", 8, 3);

    // Lignes de points avec sprites
    int sprite_x = 9 * 5 * g->pixel_size; // Colonne fixe pour sprites

    uint32_t frame_time = SDL_GetTicks() / 500;
    int frame = frame_time % 2;

    // UFO (Mystery)
    si_text_display(g, "=? MYSTERY", 10, 10);
    si_ufo_display(g, sprite_x, 10 * 8 * g->pixel_size);

    // SQUID (30 points)
    si_text_display(g, "=30 POINTS", 12, 10);
    si_invader_display(g, SI_TYPE_SQUID, frame, sprite_x, 12 * 8 * g->pixel_size);

    // CRAB (20 points)
    si_text_display(g, "=20 POINTS", 14, 10);
    si_invader_display(g, SI_TYPE_CRAB, frame, sprite_x, 14 * 8 * g->pixel_size);

    // OCTOPUS (10 points)
    si_text_display(g, "=10 POINTS", 16, 10);
    si_invader_display(g, SI_TYPE_OCTOPUS, frame, sprite_x, 16 * 8 * g->pixel_size);

    // Credit - position corrigée
    si_text_display(g, "CREDIT 00", 20, 9);

    // Instructions - position corrigée
    si_text_display(g, "PLAY", 23, 11);
    si_text_display(g, "<PRESS SPACE>", 24, 7);
}
void game_over(Game *g)
{
    SDL_SetRenderDrawColor(g->ren, 0x20, 0x00, 0x20, 0xFF);
    SDL_RenderClear(g->ren);

    si_text_display(g, "GAME OVER", 10, 9);

    char final_score[30];
    snprintf(final_score, sizeof(final_score), "SCORE: %05d", g->si->score_1);
    si_text_display(g, final_score, 12, 9);

    char high_score[30];
    snprintf(high_score, sizeof(high_score), "HI-SCORE: %05d", g->si->score_highest);
    si_text_display(g, high_score, 14, 8);

    char wave_text[30];
    snprintf(wave_text, sizeof(wave_text), "WAVE: %d", g->si->invaders.wave_number);
    si_text_display(g, wave_text, 16, 9);

    si_text_display(g, "PRESS SPACE TO PLAY AGAIN", 18, 4);
    si_text_display(g, "PRESS ESC TO QUIT", 20, 6);

    int frame = (SDL_GetTicks() / 200) % 2;
    for (int i = 0; i < 3; i++) {
        si_tank_explode_display(g, frame, 150 + i * 150, 250);
        si_invader_explode_display(g, 200 + i * 150, 300);
    }
}