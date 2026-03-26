/*
 * game.h
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_
#include <stdint.h>
#include "monitor.h"
#include "controls.h"

#define MAX_ENEMIES 100
#define MAX_BULLETS 500
#define FIRE_COOLDOWN_MS 200 // 5 bullets per second (1000ms / 5)
#define BULLET_SPEED 5

typedef struct
{
  int x;
  int y;
} int2d;

typedef struct
{
  int2d p;
  uint8_t health;
  int2d velocity;
  uint8_t cooldown;
} player;

typedef struct
{
  int2d p;
  uint8_t health;
  int2d velocity;
} enemy;

typedef enum
{
  SMALL = 0,
  MEDIUM,
  LARGE
} enemy_type;

typedef struct
{
  int2d p;
  uint8_t damage;
  uint8_t speed;
} bullet;

extern enemy enemies[MAX_ENEMIES];
extern bullet bullets[MAX_BULLETS];
extern player player1;

void draw_enemy(frame *f, enemy *e);
void draw_player(frame *f, player *p);
void draw_bullet(frame *f, bullet *b);

void draw_all(frame *f);
void update_all();
void update_player(player *p, controls *c);

void init_bullets();
void update_bullets();
void handle_shooting(player *p, controls *c, uint32_t current_time_ms);

void init_test_enemy();
bool check_AABB(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void update_enemy();

#endif /* INC_GAME_H_ */
