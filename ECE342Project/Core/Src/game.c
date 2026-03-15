/*
 * game.c
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#include "game.h"
#include "sprite.h"

enemy enemies[MAX_ENEMIES];
bullet bullets[MAX_BULLETS];
player player1;

void
draw_enemy (frame *f, enemy *e)
{
  write_pixel (f, e->p.x, e->p.y, RED);
}

void
draw_player (frame *f, player *p)
{
  draw_sprite (f, p->p.x, p->p.y, jet_sprite, 6, 5, 5);
}

void
draw_bullet (frame *f, bullet *b)
{
  write_pixel (f, b->p.x, b->p.y, WHITE);
}

void
draw_all (frame *f)
{
  draw_player (f, &player1);
  for (int i = 0; i < MAX_ENEMIES; i++)
    {
      if (enemies[i].health > 0)
	draw_enemy (f, &enemies[i]);
    }
  for (int i = 0; i < MAX_BULLETS; i++)
    {
      if (bullets[i].damage > 0)
	draw_bullet (f, &bullets[i]);
    }
}

