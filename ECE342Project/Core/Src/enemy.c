#include "enemy.h"
#include "player.h"

enemy enemies[MAX_ENEMIES];
bullet bullets[MAX_BULLETS];

void draw_enemy(frame *f, enemy *e)
{
  write_pixel(f, e->p.x, e->p.y, RED);
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
  int bullet_w = 1;
  int bullet_h = 3;
  int enemy_w = 6; // Adjust if your enemy sprite is a different size
  int enemy_h = 5;

  // Loop through all active bullets
  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
  {
    if (player_bullets[i].damage > 0)
    {
      // For each active bullet, check against all active enemies
      for (int j = 0; j < MAX_ENEMIES; j++)
      {
        if (enemies[j].health > 0)
        {
          // Check for overlap
          if (check_AABB(player_bullets[i].p.x, player_bullets[i].p.y, bullet_w, bullet_h,
                         enemies[j].p.x, enemies[j].p.y, enemy_w, enemy_h))
          {
            // Collision hit!
            enemies[j].health -= 1; // Reduce enemy health (destroys it if health hits 0)
            player_bullets[i].damage = 0;  // Deactivate the bullet

            // Bullet is destroyed, so stop checking it against other enemies
            break;
          }
        }
      }
    }
  }
}