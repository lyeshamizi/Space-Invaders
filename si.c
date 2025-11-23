#include "si.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

// Nouvelle fonction de collision précise
static int si_precise_collision(int x1, int y1, int w1, int h1,
                               int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2 && x1 + w1 > x2 &&
            y1 < y2 + h2 && y1 + h1 > y2);
}

Si *si_new(int window_width, int window_height, int pixel_size)
{
    Si *si = malloc(sizeof(Si));
    if (!si) return NULL;

    si->window_width = window_width;
    si->window_height = window_height;
    si->pixel_size = pixel_size;
    si->nbr_players = 1;
    si->score_1 = 0;
    si->life_1 = 3;
    si->score_2 = 0;
    si->life_2 = 0;
    si->score_highest = 0;
    si->game_over = 0;
    si->victory = 0;
    si->last_ufo_sound_time = 0;

    si->tank.x = window_width / 2;
    si->tank.y = window_height - 25 * pixel_size;
    si->tank.firing = 0;
    si->tank.shoot_x = 0;
    si->tank.shoot_y = 0;
    si->tank.destroyed = 0;
    si->tank.destroyed_count = 0;
    si->tank.width = 13 * pixel_size;
    si->tank.height = 8 * pixel_size;
    si->tank.lives = si->life_1;

    si->invaders.x = 50;
    si->invaders.y = 80;
    si->invaders.direction = 1;
    si->invaders.firing = 0;
    si->invaders.bomb_x = 0;
    si->invaders.bomb_y = 0;
    si->invaders.last_move_time = 0;
    si->invaders.last_animation_time = 0;
    si->invaders.current_frame = 0;
    si->invaders.alive_count = 0;
    si->invaders.wave_number = 1;
    si->invaders.speed_level = 1;
    si->invaders.base_move_interval = 1000;
    si->invaders.move_interval = 1000;
    si->invaders.descent_step = 8 * pixel_size;

    si->ufo.x = 0;
    si->ufo.y = 30 * pixel_size;
    si->ufo.active = 0;
    si->ufo.direction = 1;
    si->ufo.speed = 2 * pixel_size;
    si->ufo.spawn_time = 0;
    si->ufo.last_spawn_check = 0;
    si->ufo.points = 0;

    for (int i = 0; i < 10; i++) {
        si->explosions[i].active = 0;
        si->explosions[i].x = 0;
        si->explosions[i].y = 0;
        si->explosions[i].type = 0;
        si->explosions[i].frame = 0;
        si->explosions[i].start_time = 0;
        si->explosions[i].duration = 500;
    }

    si_init_matrix(si);

    return si;
}

void si_del(Si *si)
{
    free(si);
}

char *si_get_matrix(Si *si)
{
    return (char *)si->invaders.matrix;
}

int si_matrix_count(Si *si)
{
    return si->invaders.alive_count;
}

void si_init_matrix(Si *si)
{
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 11; j++) {
            if (i == 0) {
                si->invaders.matrix[i][j] = SI_TYPE_SQUID;
            } else if (i == 1 || i == 2) {
                si->invaders.matrix[i][j] = SI_TYPE_CRAB;
            } else {
                si->invaders.matrix[i][j] = SI_TYPE_OCTOPUS;
            }
        }
    }
    si->invaders.alive_count = 5 * 11;
}

void si_reset_matrix(Si *si)
{
    si_init_matrix(si);
    si->invaders.x = 50;
    si->invaders.y = 80;
    si->invaders.direction = 1;
    si->invaders.firing = 0;
}

int si_get_points(Si_Type t)
{
    switch (t) {
        case SI_TYPE_SQUID: return 30;
        case SI_TYPE_CRAB: return 20;
        case SI_TYPE_OCTOPUS: return 10;
        default: return 0;
    }
}

void si_add_score(Si *si, int points)
{
    si->score_1 += points;
    if (si->score_1 > si->score_highest) {
        si->score_highest = si->score_1;
    }
}

void si_reset_score(Si *si)
{
    si->score_1 = 0;
}

int si_get_nbr_players(Si *si)
{
    return si ? si->nbr_players : 0;
}

void si_tank_lose_life(Si *si)
{
    if (si->tank.lives > 0) {
        si->tank.lives--;
        si->life_1 = si->tank.lives;
        si_add_explosion(si, si->tank.x + si->tank.width/2, si->tank.y + si->tank.height/2, 1);
    }

    if (si->tank.lives <= 0) {
        si_set_game_over(si);
    }
}

int si_tank_has_lives(Si *si)
{
    return si->tank.lives > 0;
}

void si_reset_tank(Si *si)
{
    si->tank.x = si->window_width / 2 - si->tank.width / 2;
    si->tank.y = si->window_height - 25 * si->pixel_size;
    si->tank.firing = 0;
    si->tank.destroyed = 0;
    si->tank.destroyed_count = 0;
}

int si_tank_shoot_can_move_up(Si *si)
{
    if (!si->tank.firing) return 0;

    si->tank.shoot_y -= 6 * si->pixel_size;

    if (si->tank.shoot_y < 0) {
        si->tank.firing = 0;
        return 0;
    }
    return 1;
}

int si_tank_is_hit(Si *si)
{
    if (!si->invaders.firing) return 0;

    int bomb_width = 5 * si->pixel_size;
    int bomb_height = 8 * si->pixel_size;

    // SUPPRIMEZ les variables inutilisées et simplifiez le calcul :
    int bomb_left = si->invaders.bomb_x;
    int bomb_right = si->invaders.bomb_x + bomb_width;
    int bomb_top = si->invaders.bomb_y;
    int bomb_bottom = si->invaders.bomb_y + bomb_height;

    int tank_left = si->tank.x;
    int tank_right = si->tank.x + si->tank.width;
    int tank_top = si->tank.y;
    int tank_bottom = si->tank.y + si->tank.height;

    if (bomb_right > tank_left && bomb_left < tank_right &&
        bomb_bottom > tank_top && bomb_top < tank_bottom) {
        si->invaders.firing = 0;
        return 1;
    }

    return 0;
}
void si_tank_shoot(Si *si)
{
    if (!si->tank.firing && !si->tank.destroyed && si_tank_has_lives(si)) {
        si->tank.firing = 1;
        si->tank.shoot_x = si->tank.x + (si->tank.width / 2);
        si->tank.shoot_y = si->tank.y;
    }
}

void si_invaders_get_column(Si *si)
{
    if (si->invaders.firing || si->invaders.alive_count == 0) return;

    int available_columns[11];
    int available_count = 0;
    for (int col = 0; col < 11; col++) {
        for (int row = 0; row < 5; row++) {
            if (si->invaders.matrix[row][col] != 0) {
                available_columns[available_count++] = col;
                break;
            }
        }
    }
    if (available_count == 0) return;

    int random_col = available_columns[rand() % available_count];
    int bottom_row = -1;
    for (int row = 4; row >= 0; row--) {
        if (si->invaders.matrix[row][random_col] != 0) {
            bottom_row = row;
            break;
        }
    }
    if (bottom_row == -1) return;

    si->invaders.firing = 1;
    int invader_width = 12 * si->pixel_size;
    si->invaders.bomb_x = si->invaders.x + random_col * (invader_width + 2 * si->pixel_size) + (invader_width / 2) - (2 * si->pixel_size);

    int invader_height = 8 * si->pixel_size;
    int row_spacing = 10 * si->pixel_size;
    si->invaders.bomb_y = si->invaders.y + bottom_row * row_spacing + invader_height;
}

int si_invaders_bomb_can_move_down(Si *si)
{
    if (!si->invaders.firing) return 0;

    si->invaders.bomb_y += 4 * si->pixel_size;

    if (si->invaders.bomb_y > si->window_height) {
        si->invaders.firing = 0;
        return 0;
    }
    return 1;
}

int si_invaders_can_move(Si *si)
{
    int invader_width = 12 * si->pixel_size;
    int spacing = 2 * si->pixel_size;
    int matrix_width = 11 * (invader_width + spacing) - spacing;

    if (si->invaders.direction == 1) {
        int right_edge = si->invaders.x + matrix_width;
        return (right_edge < si->window_width - 20);
    } else {
        return (si->invaders.x > 20);
    }
}

int si_should_descend(Si *si)
{
    int invader_width = 12 * si->pixel_size;
    int spacing = 2 * si->pixel_size;
    int matrix_width = 11 * (invader_width + spacing) - spacing;

    if (si->invaders.direction == 1) {
        int right_edge = si->invaders.x + matrix_width;
        return (right_edge >= si->window_width - 20);
    } else {
        return (si->invaders.x <= 20);
    }
}

void si_descend_invaders(Si *si)
{
    si->invaders.y += si->invaders.descent_step;
    si->invaders.direction *= -1;
}

void si_move_invaders(Si *si)
{
    if (si_should_descend(si)) {
        si_descend_invaders(si);
    } else {
        int move_speed = 2 * si->pixel_size;
        si->invaders.x += si->invaders.direction * move_speed;
    }

    static int last_alive_count = 55;
    if (si->invaders.alive_count != last_alive_count) {
        float speed_factor = 1.0f + (55.0f - si->invaders.alive_count) / 55.0f * 1.5f; // Réduit de 2.0 à 1.5
        si->invaders.move_interval = si->invaders.base_move_interval / speed_factor;
        last_alive_count = si->invaders.alive_count;
    }
}

void si_invaders_shoot(Si *si)
{
    if (si->invaders.alive_count > 0 && !si->invaders.firing) {
        // Probabilité basée sur le nombre d'envahisseurs restants
        int probability = 500 + (55 - si->invaders.alive_count) * 10;

        if (probability < 100) probability = 100;
        if (probability > 1000) probability = 1000;

        if (rand() % probability == 0) {
            si_invaders_get_column(si);
        }
    }
}

void si_update_invaders_animation(Si *si)
{
    uint32_t current_time = SDL_GetTicks();
    if (current_time - si->invaders.last_animation_time > 500) {
        si->invaders.current_frame = 1 - si->invaders.current_frame;
        si->invaders.last_animation_time = current_time;
    }
}

void si_next_wave(Si *si)
{
    si->invaders.wave_number++;

    // Augmentation progressive de la difficulté
    float speed_increase = 1.0f + (si->invaders.wave_number - 1) * 0.15f;
    si->invaders.base_move_interval = 1000 / speed_increase;

    // Limiter la vitesse minimale
    if (si->invaders.base_move_interval < 300) {
        si->invaders.base_move_interval = 300;
    }

    si->invaders.move_interval = si->invaders.base_move_interval;
    si_reset_matrix(si);

    // Réinitialiser la position
    si->invaders.x = 50;
    si->invaders.y = 80;
    si->invaders.direction = 1;

    si_ufo_reset(si);
}

void si_increase_speed(Si *si)
{
    // Réduit l'intervalle de mouvement de 5%
    si->invaders.base_move_interval = (int)(si->invaders.base_move_interval * 0.95f);
    if (si->invaders.base_move_interval < 200) {
        si->invaders.base_move_interval = 200;
    }
    si->invaders.move_interval = si->invaders.base_move_interval;
}

void si_ufo_spawn(Si *si)
{
    if (si->ufo.active) return;

    uint32_t current_time = SDL_GetTicks();
    if (current_time - si->ufo.last_spawn_check > 10000) {
        if (rand() % 100 < 30) {
            si->ufo.active = 1;
            si->ufo.direction = (rand() % 2) ? 1 : -1;
            si->ufo.speed = 3 * si->pixel_size;
            si->ufo.spawn_time = current_time;

            if (si->ufo.direction == 1) {
                si->ufo.x = -20 * si->pixel_size;
            } else {
                si->ufo.x = si->window_width + 20 * si->pixel_size;
            }

            si->ufo.y = 25 * si->pixel_size;

            int point_values[] = {50, 100, 150, 200, 300};
            si->ufo.points = point_values[rand() % 5];

            si->ufo.last_spawn_check = current_time;
        } else {
            si->ufo.last_spawn_check = current_time;
        }
    }
}

void si_ufo_move(Si *si)
{
    if (!si->ufo.active) return;

    si->ufo.x += si->ufo.direction * si->ufo.speed;

    int ufo_width = 16 * si->pixel_size;
    if ((si->ufo.direction == 1 && si->ufo.x > si->window_width + ufo_width) ||
        (si->ufo.direction == -1 && si->ufo.x < -ufo_width)) {
        si->ufo.active = 0;
    }

    uint32_t current_time = SDL_GetTicks();
    if (current_time - si->ufo.spawn_time > 15000) {
        si->ufo.active = 0;
    }
}

void si_ufo_despawn(Si *si)
{
    si->ufo.active = 0;
}

int si_ufo_is_active(Si *si)
{
    return si->ufo.active;
}

int si_ufo_is_hit(Si *si)
{
    if (!si->tank.firing || !si->ufo.active) return 0;

    int shoot_width = si->pixel_size;
    int shoot_height = 8 * si->pixel_size;

    // SUPPRIMEZ les variables inutilisées :
    int shoot_left = si->tank.shoot_x;
    int shoot_right = si->tank.shoot_x + shoot_width;
    int shoot_top = si->tank.shoot_y;
    int shoot_bottom = si->tank.shoot_y + shoot_height;

    int ufo_width = 16 * si->pixel_size;
    int ufo_height = 8 * si->pixel_size;

    int ufo_left = si->ufo.x;
    int ufo_right = si->ufo.x + ufo_width;
    int ufo_top = si->ufo.y;
    int ufo_bottom = si->ufo.y + ufo_height;

    if (shoot_right > ufo_left && shoot_left < ufo_right &&
        shoot_bottom > ufo_top && shoot_top < ufo_bottom) {

        si_add_score(si, si->ufo.points);
        si_add_explosion(si, si->ufo.x + ufo_width/2, si->ufo.y + ufo_height/2, 0);
        si->tank.firing = 0;
        si->ufo.active = 0;
        return 1;
    }

    return 0;
}

void si_ufo_reset(Si *si)
{
    si->ufo.active = 0;
    si->ufo.last_spawn_check = 0;
}

void si_add_explosion(Si *si, int x, int y, int type)
{
    static int last_index = 0;
    int start_index = (last_index + 1) % 10;

    for (int i = 0; i < 10; i++) {
        int index = (start_index + i) % 10;
        if (!si->explosions[index].active) {
            si->explosions[index].x = x;
            si->explosions[index].y = y;
            si->explosions[index].type = type;
            si->explosions[index].frame = 0;
            si->explosions[index].start_time = SDL_GetTicks();
            si->explosions[index].active = 1;
            si->explosions[index].duration = (type == 0) ? 300 : 500;
            last_index = index;
            break;
        }
    }
}

void si_update_explosions(Si *si)
{
    uint32_t current_time = SDL_GetTicks();
    int active_count = 0;

    for (int i = 0; i < 10; i++) {
        if (si->explosions[i].active) {
            uint32_t elapsed = current_time - si->explosions[i].start_time;

            // Mettre à jour seulement si nécessaire
            if (elapsed > si->explosions[i].duration) {
                si->explosions[i].active = 0;
            } else {
                si->explosions[i].frame = (elapsed / 100) % 2;
                active_count++;
            }
        }
    }

    // Limiter le nombre d'explosions simultanées
    if (active_count > 5) {
        for (int i = 0; i < 10 && active_count > 5; i++) {
            if (si->explosions[i].active) {
                si->explosions[i].active = 0;
                active_count--;
            }
        }
    }
}

void si_clear_explosions(Si *si)
{
    for (int i = 0; i < 10; i++) {
        si->explosions[i].active = 0;
    }
}

int si_check_tank_hit(Si *si)
{
    return si_tank_is_hit(si);
}

int si_check_invader_hit(Si *si)
{
    if (!si->tank.firing) return 0;

    int shoot_width = si->pixel_size;
    int shoot_height = 8 * si->pixel_size;

    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 11; col++) {
            if (si->invaders.matrix[row][col] != 0) {
                int invader_width = 12 * si->pixel_size;
                int invader_height = 8 * si->pixel_size;
                int row_spacing = 10 * si->pixel_size;
                int col_spacing = 2 * si->pixel_size;

                int invader_x = si->invaders.x + col * (invader_width + col_spacing);
                int invader_y = si->invaders.y + row * row_spacing;

                // Collision précise
                if (si_precise_collision(si->tank.shoot_x, si->tank.shoot_y,
                                       shoot_width, shoot_height,
                                       invader_x, invader_y,
                                       invader_width, invader_height)) {
                    si_remove_invader(si, row, col);
                    si->tank.firing = 0;
                    si_add_explosion(si, invader_x + invader_width/2,
                                   invader_y + invader_height/2, 0);
                    return 1;
                }
            }
        }
    }
    return 0;
}

int si_check_ufo_hit(Si *si)
{
    return si_ufo_is_hit(si);
}

void si_remove_invader(Si *si, int row, int col)
{
    if (row < 0 || row >= 5 || col < 0 || col >= 11) return;

    if (si->invaders.matrix[row][col] != 0) {
        Si_Type type = (Si_Type)si->invaders.matrix[row][col];
        int points = si_get_points(type);
        si_add_score(si, points);

        si->invaders.matrix[row][col] = 0;
        si->invaders.alive_count--;

        // Augmentation de vitesse progressive
        if (si->invaders.alive_count > 0) {
            float speed_factor = 1.0f + (55.0f - si->invaders.alive_count) / 55.0f * 1.5f;
            si->invaders.move_interval = si->invaders.base_move_interval / speed_factor;
        }
    }
}

int si_are_all_invaders_dead(Si *si)
{
    return si->invaders.alive_count == 0;
}

int si_have_invaders_reached_bottom(Si *si)
{
    int bottom_of_matrix = si->invaders.y + 5 * 10 * si->pixel_size;
    int danger_zone = si->tank.y - 10 * si->pixel_size;
    return bottom_of_matrix >= danger_zone;
}

int si_is_game_over(Si *si)
{
    return si->game_over;
}

void si_set_game_over(Si *si)
{
    si->game_over = 1;
}

void si_check_victory(Si *si)
{
    si->victory = si_are_all_invaders_dead(si);
}

void si_reset_game(Si *si)
{
    si->score_1 = 0;
    si->life_1 = 3;
    si->tank.lives = 3;
    si->game_over = 0;
    si->victory = 0;

    si_reset_tank(si);
    si_ufo_reset(si);
    si_clear_explosions(si);

    si->invaders.wave_number = 1;
    si->invaders.speed_level = 1;
    si->invaders.base_move_interval = 1000;
    si->invaders.move_interval = 1000;
    si_reset_matrix(si);
}