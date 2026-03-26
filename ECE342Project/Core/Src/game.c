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
uint32_t last_fire_time = 0;

void draw_enemy(frame *f, enemy *e)
{
  write_pixel(f, e->p.x, e->p.y, RED);
}

void draw_player(frame *f, player *p)
{
  draw_sprite(f, p->p.x % IMG_COL, p->p.y % IMG_ROW, jet_sprite, 6, 5, 5);
}

void draw_bullet(frame *f, bullet *b)
{
  // write_pixel(f, b->p.x, b->p.y, WHITE);
  draw_sprite(f, b->p.x, b->p.y, bullet_sprite, 2, 1, 3);
}

void draw_all(frame *f)
{
  draw_player(f, &player1);
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
}

void update_player(player *p, controls *c)
{
  update_controls(c);
  p->velocity.x = c->joystick_x * 10; // scale the joystick input to control speed
  p->velocity.y = c->joystick_y * 10;
  p->p.x += p->velocity.x;
  p->p.y += p->velocity.y;

  if (p->p.x >= IMG_COL - 5)
  {
    p->p.x = IMG_COL - 5;
  }
  if (p->p.y >= IMG_ROW - 5)
  {
    p->p.y = IMG_ROW - 5;
  }
  // also set min vals to 0
  if (p->p.x < 5)
  {
    p->p.x = 5;
  }
  if (p->p.y < 5)
  {
    p->p.y = 5;
  }
}

void init_bullets()
{
  for (int i = 0; i < MAX_BULLETS; i++)
  {
    bullets[i].damage = 0;
  }
}

// Pass in your current system time (e.g., HAL_GetTick())
// LATER: CREATE ANOTHER ARRAY FOR PLAYER BULLETS
void handle_shooting(player *p, controls *c, uint32_t current_time_ms)
{
  if (c->button_shoot)
  {
    // Check if 200ms have passed since the last shot
    if ((current_time_ms - last_fire_time) >= FIRE_COOLDOWN_MS)
    {
      for (int i = 0; i < MAX_BULLETS; i++)
      {
        // We use damage == 0 to represent an inactive bullet
        if (bullets[i].damage == 0)
        {
          bullets[i].damage = 1;           // Set damage to activate it
          bullets[i].speed = BULLET_SPEED; 
          bullets[i].p.x = p->p.x + 2;     // Center bullet on the jet (adjust offset as needed)
          bullets[i].p.y = p->p.y - 3;     // Spawn slightly above the jet

          last_fire_time = current_time_ms; 
          break;                            // Stop searching after spawning one bullet
        }
      }
    }
  }
}

void update_bullets()
{
  for (int i = 0; i < MAX_BULLETS; i++)
  {
    if (bullets[i].damage > 0)
    {
      bullets[i].p.y -= bullets[i].speed; // Move the bullet up the screen

      // Deactivate the bullet if it goes off the top edge of the screen
      // Assuming 0 is the top boundary
      if (bullets[i].p.y < 0 || bullets[i].p.x < 0 || bullets[i].p.x > IMG_COL)
      {
        bullets[i].damage = 0;
      }
    }
  }
}

void init_test_enemy()
{
  // Make sure all enemies are cleared out first
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    enemies[i].health = 0;
  }

  // Spawn one active enemy at a fixed location
  enemies[0].p.x = IMG_COL / 2 - 3; // Roughly centered horizontally
  enemies[0].p.y = 20;
  enemies[0].health = 1; // 1 hit point
  enemies[0].velocity.x = 0;
  enemies[0].velocity.y = 0;
}

bool check_AABB(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
  return (x1 < x2 + w2 && // Box 1's left edge is past Box 2's right edge
          x1 + w1 > x2 && // Box 1's right edge is past Box 2's left edge
          y1 < y2 + h2 && // Box 1's top edge is past Box 2's bottom edge
          y1 + h1 > y2);  // Box 1's bottom edge is past Box 2's top edge
}

// Use AABB to Check whether Enemies collide with bullets
void update_enemy()
{
  // Define the bounding box dimensions based on your sprites
  int bullet_w = 2;
  int bullet_h = 3;
  int enemy_w = 6; // Adjust if your enemy sprite is a different size
  int enemy_h = 5;

  // Loop through all active bullets
  for (int i = 0; i < MAX_BULLETS; i++)
  {
    if (bullets[i].damage > 0)
    {
      // For each active bullet, check against all active enemies
      for (int j = 0; j < MAX_ENEMIES; j++)
      {
        if (enemies[j].health > 0)
        {
          // Check for overlap
          if (check_AABB(bullets[i].p.x, bullets[i].p.y, bullet_w, bullet_h,
                         enemies[j].p.x, enemies[j].p.y, enemy_w, enemy_h))
          {
            // Collision hit!
            enemies[j].health -= 1; // Reduce enemy health (destroys it if health hits 0)
            bullets[i].damage = 0;  // Deactivate the bullet

            // Bullet is destroyed, so stop checking it against other enemies
            break;
          }
        }
      }
    }
  }
}