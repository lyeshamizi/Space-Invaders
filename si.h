#ifndef SI_H
#define SI_H

#include <stdint.h>

typedef struct Invaders Invaders;
typedef struct Tank Tank;
typedef struct Ufo Ufo;
typedef struct Explosion Explosion;
typedef struct Si Si;

typedef enum {
    SI_TYPE_SQUID = 1,
    SI_TYPE_CRAB,
    SI_TYPE_OCTOPUS,
} Si_Type;

struct Explosion {
    int x;
    int y;
    int active;
    int type;
    int frame;
    uint32_t start_time;
    uint32_t duration;
};

struct Ufo {
    int x;
    int y;
    int active;
    int direction;
    int speed;
    uint32_t spawn_time;
    uint32_t last_spawn_check;
    int points;
};

struct Invaders {
    int x;
    int y;
    int direction;
    int firing;
    int bomb_x;
    int bomb_y;
    char matrix[5][11];
    int alive_count;
    uint32_t last_move_time;
    uint32_t last_animation_time;
    int current_frame;
    int move_interval;
    int base_move_interval;
    int descent_step;
    int wave_number;
    int speed_level;
};

struct Tank {
    int x;
    int y;
    int firing;
    int shoot_x;
    int shoot_y;
    int destroyed;
    int destroyed_count;
    int width;
    int height;
    int lives;
};

struct Si {
    int window_width;
    int window_height;
    int pixel_size;
    int nbr_players;
    int score_1;
    int life_1;
    int score_2;
    int life_2;
    int score_highest;
    Tank tank;
    Invaders invaders;
    Ufo ufo;
    Explosion explosions[10];
    int game_over;
    int victory;
    uint32_t last_ufo_sound_time;
};

Si *si_new(int window_width, int window_height, int pixel_size);
void si_del(Si *si);

char *si_get_matrix(Si *si);
int si_matrix_count(Si *si);
void si_init_matrix(Si *si);
void si_reset_matrix(Si *si);

int si_get_points(Si_Type t);
int si_get_nbr_players(Si *si);
void si_add_score(Si *si, int points);
void si_reset_score(Si *si);

int si_tank_shoot_can_move_up(Si *si);
int si_tank_is_hit(Si *si);
void si_tank_shoot(Si *si);
void si_tank_lose_life(Si *si);
int si_tank_has_lives(Si *si);
void si_reset_tank(Si *si);

void si_invaders_get_column(Si *si);
int si_invaders_bomb_can_move_down(Si *si);
int si_invaders_can_move(Si *si);
void si_move_invaders(Si *si);
void si_invaders_shoot(Si *si);
void si_update_invaders_animation(Si *si);
int si_should_descend(Si *si);
void si_descend_invaders(Si *si);
void si_next_wave(Si *si);
void si_increase_speed(Si *si);

void si_ufo_spawn(Si *si);
void si_ufo_move(Si *si);
void si_ufo_despawn(Si *si);
int si_ufo_is_active(Si *si);
int si_ufo_is_hit(Si *si);
void si_ufo_reset(Si *si);

void si_add_explosion(Si *si, int x, int y, int type);
void si_update_explosions(Si *si);
void si_clear_explosions(Si *si);

int si_check_tank_hit(Si *si);
int si_check_invader_hit(Si *si);
int si_check_ufo_hit(Si *si);
void si_remove_invader(Si *si, int row, int col);

int si_are_all_invaders_dead(Si *si);
int si_have_invaders_reached_bottom(Si *si);
int si_is_game_over(Si *si);
void si_set_game_over(Si *si);
void si_check_victory(Si *si);
void si_reset_game(Si *si);

void si_optimize_matrix(Si *si);

#endif