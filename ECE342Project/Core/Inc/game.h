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
void update_player(player *p, controls *c);

#endif /* INC_GAME_H_ */
