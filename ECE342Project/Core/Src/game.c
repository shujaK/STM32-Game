/*
 * game.c
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#include <cstdint>
#include <stdbool.h>
#include <stdint.h>
#include "game.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include "monitor.h"
#include "controls.h"

static uint32_t rng_state = 0x9E3779B9u; // Non-zero default seed
static uint32_t total_enemies;

uint16_t score = 0;
bool game_start = false;

void draw_all(frame *f, player *p)
{
  draw_player(f, p);
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    if (enemies[i].health > 0)
      draw_enemy(f, &enemies[i]);
  }
  for (int i = 0; i < MAX_BULLETS; i++)
  {
    if (bullets[i].damage > 0)
      draw_bullet(f, &bullets[i]);
  }

  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
  {
    if (player_bullets[i].damage > 0)
      draw_bullet(f, &player_bullets[i]);
  }
}

void update_all(player *p, controls *c){
  if (c->button_shoot)
  {
    game_start = true;
  }
  if (game_start)
  {
    update_player(p, c);
    update_bullets();
    update_enemy();
  }
}

void game_random_seed(uint32_t seed)
{
  rng_state = seed ? seed : 0x9E3779B9u;
}

uint32_t game_random_u32(void)
{
  uint32_t x = rng_state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  rng_state = x;
  return x;
}

int game_random_range(int min, int max)
{
  if (max <= min)
    return min;
  uint32_t span = (uint32_t)(max - min + 1);
  return (int)(game_random_u32() % span) + min;
}