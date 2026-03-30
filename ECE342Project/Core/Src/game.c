/*
 * game.c
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#include <stdbool.h>
#include <stdint.h>
#include "game.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"
#include "monitor.h"
#include "controls.h"
#include "sprite.h"

static uint32_t rng_state = 0x9E3779B9u; // Non-zero default seed

uint16_t score = 0;
bool game_start = false;

void
draw_all (frame *f, player *p)
{
  draw_player (f, p);
  // enemies
  for (int i = 0; i < MAX_ENEMIES; i++)
    {
      if (enemies[i].health > 0)
	draw_enemy (f, &enemies[i]);
    }
  // enemy bullets
  for (int i = 0; i < MAX_BULLETS; i++)
    {
      if (enemy_bullets[i].damage > 0)
	draw_bullet (f, &enemy_bullets[i]);
    }
  // player bullets
  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
    {
      if (player_bullets[i].damage > 0)
	draw_player_bullet (f, &player_bullets[i]);
    }
  // health points
  for (int i = 0; i < p->health; i++)
    {
      draw_health (f, 10 + i * (HEALTH_SPRITE_WIDTH + 2), (IMG_ROW - 15));
  }

  if (p->special_available) draw_special(f);
}

void
draw_health (frame *f, int x, int y)
{
  draw_sprite (f, x, y, health_sprite, HEALTH_SPRITE_WIDTH_PACKED,
  HEALTH_SPRITE_WIDTH,
	       HEALTH_SPRITE_HEIGHT);
}

void
draw_special (frame *f)
{
  if (!f)
    return;

  int x = IMG_COL - 10 - (SPECIAL_SPRITE_WIDTH / 2);
  int y = IMG_ROW - 10 - (SPECIAL_SPRITE_HEIGHT / 2);

  draw_sprite (f, x, y, special_sprite, SPECIAL_SPRITE_WIDTH_PACKED,
  SPECIAL_SPRITE_WIDTH, SPECIAL_SPRITE_HEIGHT);
}

void
update_all (player *p, controls *c)
{
  if (c->button_shoot)
    {
      game_start = true;
    }
  if (game_start)
    {
      update_player (p, c);
      if (p->health < 1)
	{
	  game_start = false;
	  return;
	}
      update_bullets ();
      update_enemy ();
    }
}

void
game_random_seed (uint32_t seed)
{
  rng_state = seed ? seed : 0x9E3779B9u;
}

uint32_t
game_random_u32 (void)
{
  uint32_t x = rng_state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  rng_state = x;
  return x;
}

int
game_random_range (int min, int max)
{
  if (max <= min)
    return min;
  uint32_t span = (uint32_t) (max - min + 1);
  return (int) (game_random_u32 () % span) + min;
}
