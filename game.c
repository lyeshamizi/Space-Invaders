#include "game.h"
#include "si.h"
#include "si_menu.h"
#include "si_sdl.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

static void render_game(Game *g)
{
// Afficher le label "SCORE" en haut
si_text_display(g, "SCORE", 0, 1);
// Afficher la valeur du score en dessous
char score_text[30];
snprintf(score_text, sizeof(score_text), "%05d", g->si->score_1);
si_text_display(g, score_text, 1, 1);

// Afficher le label "HI-SCORE" en haut
si_text_display(g, "HI-SCORE", 0, 10);
// Afficher la valeur du high-score en dessous
char high_text[30];
snprintf(high_text, sizeof(high_text), "%05d", g->si->score_highest);
si_text_display(g, high_text, 1, 10);

// Afficher le label "WAVE" en haut
si_text_display(g, "WAVE", 0, 23);
// Afficher la valeur de la vague en dessous
char wave_text[20];
snprintf(wave_text, sizeof(wave_text), "%d", g->si->invaders.wave_number);
si_text_display(g, wave_text, 1, 23);

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 11; col++) {
            if (g->si->invaders.matrix[row][col] != 0) {
                Si_Type type = (Si_Type)g->si->invaders.matrix[row][col];

                int invader_width = 12 * g->pixel_size;
                int col_spacing = 2 * g->pixel_size;
                int row_spacing = 10 * g->pixel_size;

                int x = g->si->invaders.x + col * (invader_width + col_spacing);
                int y = g->si->invaders.y + row * row_spacing;

                si_invader_display(g, type, g->si->invaders.current_frame, x, y);
            }
        }
    }

    if (g->si->invaders.firing) {
        si_invader_shoot_display(g, g->si->invaders.bomb_x, g->si->invaders.bomb_y);
    }

    if (si_ufo_is_active(g->si)) {
        si_ufo_display(g, g->si->ufo.x, g->si->ufo.y);
    }

    for (int i = 0; i < 10; i++) {
        if (g->si->explosions[i].active) {
            si_explosion_display(g, g->si->explosions[i].x, g->si->explosions[i].y,
                               g->si->explosions[i].type, g->si->explosions[i].frame);
        }
    }

    if (g->si->tank.destroyed) {
        si_tank_display(g, g->si->tank.x, g->si->tank.y);
    } else {
        si_tank_display(g, g->si->tank.x, g->si->tank.y);

        for (int i = 1; i < g->si->tank.lives; i++) {
            int life_x = 20 + (i-1) * 20;
            int life_y = g->window_height - 10 * g->pixel_size;
            SDL_Rect save;
            SDL_RenderGetViewport(g->ren, &save);
            SDL_RenderSetViewport(g->ren, &(SDL_Rect){life_x, life_y, 13*g->pixel_size/2, 8*g->pixel_size/2});
            si_tank_display(g, 0, 0);
            SDL_RenderSetViewport(g->ren, &save);
        }
    }

    if (g->si->tank.firing) {
        si_tank_shoot_display(g, g->si->tank.shoot_x, g->si->tank.shoot_y);
    }

    if (g->si->invaders.wave_number > 1 && SDL_GetTicks() - g->si->invaders.last_move_time < 2000) {
        char new_wave_text[30];
        snprintf(new_wave_text, sizeof(new_wave_text), "WAVE %d!", g->si->invaders.wave_number);
        si_text_display(g, new_wave_text, 8, 10);
    }
}

static void game_update(Game *g)
{
    SDL_SetRenderDrawColor(g->ren, 0x00, 0x00, 0x20, 0xFF);
    SDL_RenderClear(g->ren);

    switch (g->game_state) {
        case STATE_MENU:
            menu(g);
            break;

        case STATE_PLAYING:
            render_game(g);
            break;

        case STATE_GAME_OVER:
            game_over(g);
            break;
    }

    SDL_RenderPresent(g->ren);
    g->update = 0;
}

static void reset_game(Si *si)
{
    si_reset_game(si);
}

Game *game_new(void)
{
    Game *g = malloc(sizeof(Game));
    if (!g) return NULL;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        free(g);
        return NULL;
    }

    srand(time(NULL));
    g->pixel_size = 3;
    g->window_width = 500;
    g->window_height = 650;

    // Création de la fenêtre
    g->win = SDL_CreateWindow("Space Invaders",
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             g->window_width,
                             g->window_height,
                             0);  // Fenêtre normale sans redimensionnement

    if (!g->win) {
        fprintf(stderr, "Erreur SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        free(g);
        return NULL;
    }

    g->ren = SDL_CreateRenderer(g->win, -1,
                               SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!g->ren) {
        g->ren = SDL_CreateRenderer(g->win, -1, SDL_RENDERER_ACCELERATED);
        if (!g->ren) {
            fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
            SDL_DestroyWindow(g->win);
            SDL_Quit();
            free(g);
            return NULL;
        }
    }

    g->game_state = STATE_MENU;
    g->freq = SDL_GetPerformanceFrequency();
    g->count_invaders = SDL_GetPerformanceCounter();
    g->count_shoot = SDL_GetPerformanceCounter();
    g->update = 1;

    g->si = si_new(g->window_width, g->window_height, g->pixel_size);
    if (!g->si) {
        fprintf(stderr, "Erreur création structure de jeu\n");
        SDL_DestroyRenderer(g->ren);
        SDL_DestroyWindow(g->win);
        SDL_Quit();
        free(g);
        return NULL;
    }

    return g;
}

void game_del(Game *g)
{
    if (!g) return;

    if (g->si) {
        si_del(g->si);
        g->si = NULL;
    }
    if (g->ren) {
        SDL_DestroyRenderer(g->ren);
        g->ren = NULL;
    }
    if (g->win) {
        SDL_DestroyWindow(g->win);
        g->win = NULL;
    }

    SDL_Quit();
    free(g);
}

void game_run(Game *g)
{
    int running = 1;
    uint32_t last_invader_move = 0;
    uint32_t last_frame_time = SDL_GetTicks();
    int frame_count = 0;
    const int TARGET_FPS = 60;
    const uint32_t frame_delay = 1000 / TARGET_FPS;

    while (running)
    {
        uint32_t frame_start = SDL_GetTicks();
        SDL_Event event;
        uint64_t current_time = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = 0;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_q:
                        case SDLK_ESCAPE:
                            running = 0;
                            break;

                        case SDLK_SPACE:
                            if (g->game_state == STATE_MENU) {
                                reset_game(g->si);
                                g->game_state = STATE_PLAYING;
                                g->update = 1;
                            }
                            else if (g->game_state == STATE_PLAYING && !g->si->tank.destroyed) {
                                si_tank_shoot(g->si);
                                g->update = 1;
                            }
                            else if (g->game_state == STATE_GAME_OVER) {
                                reset_game(g->si);
                                g->game_state = STATE_PLAYING;
                                g->update = 1;
                            }
                            break;
                    }
                    break;

                case SDL_MOUSEMOTION:
                    if (g->game_state == STATE_PLAYING && !g->si->tank.destroyed) {
                        int mouse_x = event.motion.x;

                        if (mouse_x < g->si->tank.width / 2) {
                            mouse_x = g->si->tank.width / 2;
                        } else if (mouse_x > g->window_width - g->si->tank.width / 2) {
                            mouse_x = g->window_width - g->si->tank.width / 2;
                        }

                        g->si->tank.x = mouse_x - g->si->tank.width / 2;
                        g->update = 1;
                    }
                    break;
            }
        }

        if (g->game_state == STATE_PLAYING) {
            uint32_t current_ticks = SDL_GetTicks();

            // Mise à jour des explosions (optimisée)
            si_update_explosions(g->si);
            si_update_invaders_animation(g->si);

            if ((int)(current_ticks - last_invader_move) > g->si->invaders.move_interval) {
                si_move_invaders(g->si);
                last_invader_move = current_ticks;
                g->update = 1;
            }

            si_ufo_spawn(g->si);
            if (si_ufo_is_active(g->si)) {
                si_ufo_move(g->si);
                g->update = 1;
            }

            si_invaders_shoot(g->si);

            if ((double)(current_time - g->count_shoot) / g->freq > 0.016) {
                if (g->si->tank.firing) {
                    if (si_tank_shoot_can_move_up(g->si)) {
                        g->update = 1;
                    }
                }

                if (g->si->invaders.firing) {
                    if (si_invaders_bomb_can_move_down(g->si)) {
                        g->update = 1;
                    }
                }

                g->count_shoot = current_time;
            }

            if (si_check_invader_hit(g->si)) {
                g->update = 1;
            }

            if (si_check_ufo_hit(g->si)) {
                g->update = 1;
            }

            if (si_check_tank_hit(g->si)) {
                g->si->tank.destroyed = 1;
                si_tank_lose_life(g->si);
                g->update = 1;
            }

            if (g->si->tank.destroyed) {
                g->si->tank.destroyed_count++;
                if (g->si->tank.destroyed_count > 60) {
                    g->si->tank.destroyed = 0;
                    g->si->tank.destroyed_count = 0;

                    if (si_tank_has_lives(g->si)) {
                        si_reset_tank(g->si);
                    } else {
                        g->game_state = STATE_GAME_OVER;
                    }
                    g->update = 1;
                }
            }

            if (si_are_all_invaders_dead(g->si)) {
                si_next_wave(g->si);
                g->update = 1;
            }

            if (si_have_invaders_reached_bottom(g->si)) {
                si_set_game_over(g->si);
                g->game_state = STATE_GAME_OVER;
                g->update = 1;
            }

            if (si_is_game_over(g->si)) {
                g->game_state = STATE_GAME_OVER;
                g->update = 1;
            }
        }

        frame_count++;
        if (SDL_GetTicks() - last_frame_time >= 1000) {
            //printf("FPS: %d\n", frame_count); // Décommentez pour debug FPS
            frame_count = 0;
            last_frame_time = SDL_GetTicks();
        }

        if (g->update)
            game_update(g);

        // Contrôle FPS
        uint32_t frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < frame_delay) {
            SDL_Delay(frame_delay - frame_time);
        }
    }
}