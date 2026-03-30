/*
 * game.h
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#ifndef INC_GAME_H_
#define INC_GAME_H_
#include <stdint.h>
#include <stdbool.h>
#include "monitor.h"  // frame
#include "controls.h" // controls

#define MAX_ENEMIES 100
#define MAX_BULLETS 500
#define MAX_PLAYER_BULLETS 20
#define FIRE_COOLDOWN_MS 200 // 5 bullets per second (1000ms / 5)
#define SPECIAL_COOLDOWN_MS 10000
#define PLAYER_BULLET_SPEED 5

typedef struct
{
  int x;
  int y;
} int2d;

// Forward declaration to allow prototypes without pulling in player.h here
typedef struct player player;

extern uint16_t score;
extern bool game_start;

// Forward declaration for prototypes without including player.h here
typedef struct player player;

void
draw_all (frame *f, player *p);
void
draw_health (frame *f, int x, int y);
void
draw_special (frame *f);
void
update_all (player *p, controls *c);

void
game_random_seed (uint32_t seed);
uint32_t
game_random_u32 (void);
int
game_random_range (int min, int max);

#endif /* INC_GAME_H_ */
