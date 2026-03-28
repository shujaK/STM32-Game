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
#include "sprite.h"

#define MAX_ENEMIES 100
#define MAX_BULLETS 500
#define MAX_PLAYER_BULLETS 20
#define FIRE_COOLDOWN_MS 200 // 5 bullets per second (1000ms / 5)
#define PLAYER_BULLET_SPEED 5

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
  uint32_t cooldown;
} player;

typedef struct
{
  int2d p;
  uint8_t health;
  int2d velocity;
  uint32_t time;
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

extern uint16_t score;

void draw_all(frame *f, player *p);
void update_all(player *p, controls *c);

void game_random_seed(uint32_t seed);
uint32_t game_random_u32(void);
int game_random_range(int min, int max);


#endif /* INC_GAME_H_ */
