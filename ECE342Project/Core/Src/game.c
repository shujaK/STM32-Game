/*
 * game.c
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#include "game.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"

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
    uint64_t current_time_ms = (uint64_t)HAL_GetTick();
    handle_shooting(p, c, current_time_ms);
    update_bullets();
    update_enemy();
  }
}