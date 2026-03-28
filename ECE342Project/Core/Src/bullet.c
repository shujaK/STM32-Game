#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "sprite.h"

void draw_bullet(frame *f, bullet *b)
{
  // write_pixel(f, b->p.x, b->p.y, WHITE);
  draw_sprite(f, b->p.x, b->p.y, bullet_sprite, 2, 1, 3);
}

void draw_player_bullet(frame *f, bullet *b)
{
  draw_sprite(f, b->p.x, b->p.y, player_bullet_sprite,
              PLAYER_BULLET_SPRITE_WIDTH_PACKED, PLAYER_BULLET_SPRITE_WIDTH, PLAYER_BULLET_SPRITE_HEIGHT);
}

void init_bullets()
{
  for (int i = 0; i < MAX_BULLETS; i++)
  {
    bullets[i].damage = 0;
  }

  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
  {
    player_bullets[i].damage = 0;
  }
}

void draw_enemy_bullet(frame *f, bullet *b)
{
  draw_sprite(f, b->p.x, b->p.y, enemy_bullet_sprite,
              ENEMY_BULLET_SPRITE_WIDTH_PACKED, ENEMY_BULLET_SPRITE_WIDTH, ENEMY_BULLET_SPRITE_HEIGHT);
}

void update_bullets()
{
  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
  {
    if (player_bullets[i].damage > 0)
    {
      player_bullets[i].p.y -= player_bullets[i].speed; // Move the bullet up the screen

      // Deactivate the bullet if it goes off the top edge of the screen
      // Assuming 0 is the top boundary
      if (player_bullets[i].p.y < 3 || player_bullets[i].p.x < 0 || player_bullets[i].p.x > IMG_COL)
      {
        player_bullets[i].damage = 0;
      }
    }
  }

  // Enemy bullets update could be added here similarly if needed
  
}