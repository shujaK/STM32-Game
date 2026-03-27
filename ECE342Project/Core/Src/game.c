/*
 * game.c
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#include "game.h"
#include "sprite.h"
#include "enemy.h"
#include "player.h"
#include "bullet.h"

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
  update_player(p, c);
  
  uint32_t current_time_ms = HAL_GetTick();
  handle_shooting(p, c, current_time_ms);
  update_bullets();
  update_enemy();
}