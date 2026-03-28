#include "enemy.h"
#include "player.h"


enemy enemies[MAX_ENEMIES];
bullet bullets[MAX_BULLETS];

void draw_enemy(frame *f, enemy *e)
{
//    write_pixel(f, e->p.x, e->p.y, RED);
    draw_sprite(f, e->p.x, e->p.y, enemy_fighter_sprite, 8, 7, 7);
}

void init_test_enemy()
{
  // Make sure all enemies are cleared out first
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    enemies[i].health = 0;
  }

  int x =  IMG_COL / 2 - 3; // Roughly centered horizontally
  for (int i = 0; i<5; i++)
  {
    enemy e;
    e.p.x = x;
    e.p.y = 20;
    e.health = 1; // 1 hit point
    e.velocity.x = 1;
    e.velocity.y = 1;
    e.time = 0;

    spawn_enemy(&e);
    x += 20;
  }
}

void spawn_enemy(enemy *e)
{
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    if (enemies[i].health <= 0)
    {
      enemies[i] = *e;
      total_enemies += 1;
      return;
    }
  }
}

// spawn a wave of enemies with random positions and velocities
void spawn_enemy_wave(int num_enemies)
{
  for (int i = 0; i < num_enemies; i++)
  {
    enemy e;
    e.p.x = game_random_range(10, IMG_COL - 10);
    e.p.y = game_random_range(10, ENEMY_SPAWN_RANGE_Y);
    e.health = 1;
    e.velocity.x = game_random_range(-1, 2);
    e.velocity.y = game_random_range(-1, 2);
    e.time = 0;

    spawn_enemy(&e);
  }
}

bool check_AABB(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
  return (x1 < x2 + w2 && // Box 1's left edge is past Box 2's right edge
          x1 + w1 > x2 && // Box 1's right edge is past Box 2's left edge
          y1 < y2 + h2 && // Box 1's top edge is past Box 2's bottom edge
          y1 + h1 > y2);  // Box 1's bottom edge is past Box 2's top edge
}

void update_enemy_bullets()
{
  for (int i = 0; i < MAX_BULLETS; i++)
  {
    if (bullets[i].damage > 0)
    {
      bullets[i].p.y += bullets[i].speed; // Move the bullet up the screen

      // Deactivate the bullet if it goes off the top edge of the screen
      // Assuming 0 is the top boundary
      if (bullets[i].p.y > IMG_ROW - 5 || bullets[i].p.x < 0 || bullets[i].p.x > IMG_COL)
      {
        bullets[i].damage = 0;
      }
    }
  }
}

void handle_enemy_shooting(enemy* e, uint32_t current_time_ms)
{
    if (current_time_ms - e->time > 30000)
    {
        // current_time_ms = 0;
        e->time = 0;
    }
    
  if (current_time_ms - e->time > 1000 + game_random_range(-500, 500))
  {
    for (int i = 0; i < MAX_BULLETS; i++)
      {
        // We use damage == 0 to represent an inactive bullet
        if (bullets[i].damage == 0)
        {
          bullets[i].damage = 1;           // Set damage to activate it
          bullets[i].speed = ENEMY_BULLET_SPEED;
          bullets[i].p.x = e->p.x + 1;     // Center bullet on the jet (adjust offset as needed)
          bullets[i].p.y = e->p.y + 3;     // Spawn slightly above the jet

          e->time = current_time_ms; 
          break;                            // Stop searching after spawning one bullet
        }
      }
  }
}

// Use AABB to Check whether Enemies collide with bullets
void update_enemy()
{
  // Define the bounding box dimensions based on your sprites
  int bullet_w = 1;
  int bullet_h = 3;
  int enemy_w = 6; // Adjust if your enemy sprite is a different size
  int enemy_h = 5;
  
  for (int i = 0; i < MAX_ENEMIES; i++)
  {
    if (enemies[i].health > 0)
    {
      if ((enemies[i].p.x > IMG_COL -  enemy_w) || enemies[i].p.x < enemy_w)
      {
        enemies[i].velocity.x *= -1;
      }

      if ((enemies[i].p.y > ENEMY_SPAWN_RANGE_Y) || enemies[i].p.y < enemy_h)
      {
        enemies[i].velocity.y *= -1;
      }

      // Apply velocity
      enemies[i].p.x += enemies[i].velocity.x;
      enemies[i].p.y += enemies[i].velocity.y;

      uint64_t curr_time = (uint64_t)HAL_GetTick();
      handle_enemy_shooting(&enemies[i], curr_time);

      // check bullet collision
      for (int j = 0; j < MAX_PLAYER_BULLETS; j++)
      {
        if (player_bullets[j].damage > 0)
        {
          if (check_AABB(player_bullets[j].p.x, player_bullets[j].p.y, bullet_w, bullet_h,
                         enemies[i].p.x, enemies[i].p.y, enemy_w, enemy_h))
          {
            // Collision hit!
            enemies[i].health -= 1; // Reduce enemy health (destroys it if health hits 0)
            player_bullets[j].damage = 0; // Deactivate the bullet

            score += 10; // Increase score for hitting an enemy
            total_enemies -= 1;

            // Bullet is destroyed, so stop checking it against other enemies
            break;
          }
        }
      }
    }
  }
  update_enemy_bullets();
}