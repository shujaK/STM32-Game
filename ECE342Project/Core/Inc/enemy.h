#ifndef INC_ENEMY_H_
#define INC_ENEMY_H_

#include "game.h"
#include "bullet.h"

typedef struct enemy
{
  int2d p;
  uint8_t health;
  int2d velocity;
  uint32_t time;
} enemy;

extern enemy enemies[MAX_ENEMIES];
extern bullet enemy_bullets[MAX_BULLETS]; // enemy bullets array
extern uint32_t total_enemies;
extern uint32_t wave_complete_time;
extern uint16_t wave_num;

#define SM_ENEMY_BB
#define ENEMY_BULLET_SPEED 3
#define ENEMY_SPAWN_RANGE_Y 100
#define INVULNERABILITY_TIME 1000

void
draw_enemy (frame *f, enemy *e);
void
init_test_enemy ();
void
spawn_enemy (enemy *e);
void
spawn_enemy_wave (int num_enemies);
void
update_enemy_bullets ();
void
handle_enemy_shooting (enemy *e, uint32_t current_time_ms);
bool
check_AABB (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void
update_enemy ();

#endif /* INC_ENEMY_H_ */
