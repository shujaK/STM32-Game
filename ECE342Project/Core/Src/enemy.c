#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "main.h"

#include "../Inc/enemy.h"
#include "../Inc/player.h"
#include "../Inc/sprite.h"
#include "../Inc/animation.h"

enemy enemies[MAX_ENEMIES];
bullet enemy_bullets[MAX_BULLETS];
uint32_t total_enemies;
uint32_t wave_complete_time;
uint16_t wave_num;

#define MAX_ACTIVE_EXPLOSIONS 16
#define EXPLOSION_FRAME_DELAY 2

typedef struct explosion_instance
{
  animation anim;
  int x;
  int y;
  bool active;
} explosion_instance;

static const uint8_t *explosion_frames[EXPLOSION_SPRITE_COUNT] =
  { explosion[0], explosion[1], explosion[2], explosion[3], explosion[4] };

static explosion_instance explosion_pool[MAX_ACTIVE_EXPLOSIONS];

static void
reset_explosion (explosion_instance *e)
{
  if (!e)
    return;
  e->active = false;
}

void
reset_enemies (void)
{
  for (int i = 0; i < MAX_ENEMIES; i++)
    {
      enemies[i].health = 0;
      enemies[i].velocity.x = 0;
      enemies[i].velocity.y = 0;
      enemies[i].p.x = 0;
      enemies[i].p.y = 0;
      enemies[i].time = HAL_GetTick ();
    }

  total_enemies = 0;
  wave_complete_time = 0;

  for (int i = 0; i < MAX_ACTIVE_EXPLOSIONS; i++)
    {
      reset_explosion (&explosion_pool[i]);
    }
}

void
spawn_enemy_explosion (int x, int y)
{
  for (int i = 0; i < MAX_ACTIVE_EXPLOSIONS; i++)
    {
      explosion_instance *slot = &explosion_pool[i];
      if (slot->active)
	continue;

      animation_init (&slot->anim, explosion_frames, EXPLOSION_SPRITE_COUNT,
      EXPLOSION_SPRITE_WIDTH_PACKED,
		      EXPLOSION_SPRITE_WIDTH,
		      EXPLOSION_SPRITE_HEIGHT,
		      EXPLOSION_FRAME_DELAY, false);
      slot->x = x;
      slot->y = y;
      slot->active = true;
      return;
    }
}

void
draw_enemy_explosions (frame *f)
{
  if (!f)
    return;

  for (int i = 0; i < MAX_ACTIVE_EXPLOSIONS; i++)
    {
      explosion_instance *slot = &explosion_pool[i];
      if (!slot->active)
	continue;

      animation_play (f, &slot->anim, slot->x, slot->y);
      if (animation_finished (&slot->anim))
	{
	  reset_explosion (slot);
	}
    }
}

void
draw_enemy (frame *f, enemy *e)
{
//    write_pixel(f, e->p.x, e->p.y, RED);
  draw_sprite (f, e->p.x, e->p.y, enemy_sprite, ENEMY_SPRITE_WIDTH_PACKED,
  ENEMY_SPRITE_WIDTH,
	       ENEMY_SPRITE_HEIGHT);
}

void
init_test_enemy ()
{
  int half_w = ENEMY_SPRITE_WIDTH / 2;
  int x = IMG_COL / 2 - half_w; // Roughly centered horizontally

  // Make sure all enemies are cleared out first
  for (int i = 0; i < MAX_ENEMIES; i++)
    {
      enemies[i].health = 0;
    }
  total_enemies = 0;

  for (int i = 0; i < 5; i++)
    {
      enemy e;
      e.p.x = x;
      e.p.y = ENEMY_SPAWN_RANGE_Y / 2;
      e.health = 1; // 1 hit point
      e.velocity.x = 1;
      e.velocity.y = 1;
      e.time = HAL_GetTick ();

      spawn_enemy (&e);
      x += ENEMY_SPRITE_WIDTH + 5;
    }
}

void
spawn_enemy (enemy *e)
{
  for (int i = 0; i < MAX_ENEMIES; i++)
    {
      if (enemies[i].health == 0)
	{
	  enemies[i] = *e;
	  total_enemies += 1;
	  return;
	}
    }
}

// spawn a wave of enemies with random positions and velocities
void
spawn_enemy_wave (int num_enemies)
{
  for (int i = 0; i < num_enemies; i++)
    {
      enemy e;
      int half_w = ENEMY_SPRITE_WIDTH / 2;
      int half_h = ENEMY_SPRITE_HEIGHT / 2;
      int max_x = IMG_COL - 1 - half_w;
      int min_x = half_w;

      // Spawn just above the visible area so enemies fly in from off-screen
      int max_y = -half_h;              // top edge just off-screen
      int min_y = -ENEMY_SPRITE_HEIGHT; // fully off-screen

      e.p.x = game_random_range (min_x, max_x);
      e.p.y = game_random_range (min_y, max_y);
      if (wave_num > 15)
	{
	  // 10% chance to be toughest (health 3), 30% medium (health 2), otherwise 1 HP
	  int roll = game_random_range (1, 10);
	  if (roll <= 1)
	    {
	      e.health = 3;
	    }
	  else if (roll <= 4)
	    {
	      e.health = 2;
	    }
	  else
	    {
	      e.health = 1;
	    }
	}
      else if (wave_num > 5)
	{
	  // 30% chance to be tougher (health 2), otherwise 1 HP
	  e.health = (game_random_range (1, 10) <= 3) ? 2 : 1;
	}
      else
	{
	  e.health = 1;
	}
      e.velocity.x = game_random_range (-1, 2);
      e.velocity.y = game_random_range (1, 3); // ensure downward motion onto screen
      e.time = HAL_GetTick ();

      spawn_enemy (&e);
    }
}

bool
check_AABB (int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
  return (x1 < x2 + w2 && // Box 1's left edge is past Box 2's right edge
      x1 + w1 > x2 && // Box 1's right edge is past Box 2's left edge
      y1 < y2 + h2 && // Box 1's top edge is past Box 2's bottom edge
      y1 + h1 > y2);  // Box 1's bottom edge is past Box 2's top edge
}

void
update_enemy_bullets ()
{
  for (int i = 0; i < MAX_BULLETS; i++)
    {
      if (enemy_bullets[i].damage > 0)
	{
	  enemy_bullets[i].p.x += enemy_bullets[i].velocity.x;
	  enemy_bullets[i].p.y += enemy_bullets[i].velocity.y;

	  double phase = (double) HAL_GetTick () / 200.0;
	  int speed_mag =
	      enemy_bullets[i].velocity.y >= 0 ?
		  enemy_bullets[i].velocity.y : -enemy_bullets[i].velocity.y;
	  enemy_bullets[i].p.x += (int) (1.2
	      * sin (phase / 0.35 + i * speed_mag));

	  // Deactivate the bullet if it goes off the top edge of the screen
	  // Assuming 0 is the top boundary
	  if (enemy_bullets[i].p.y
	      > IMG_ROW
		  - 5|| enemy_bullets[i].p.x < 0 || enemy_bullets[i].p.x > IMG_COL)
	    {
	      enemy_bullets[i].damage = 0;
	    }
	}
    }
}

void
handle_enemy_shooting (enemy *e, uint32_t current_time_ms)
{
  if (current_time_ms - e->time > 30000)
    {
      // current_time_ms = 0;
      e->time = 0;
    }

  uint32_t cooldown_ms = 1000 + game_random_range (-500, 500);
  if (e->health == 2)
    {
      cooldown_ms = 5000 + game_random_range (-1000, 1000); // 5 second burst cooldown for tougher enemies
    }

  if (current_time_ms - e->time > cooldown_ms)
    {
      // Tougher enemies (health == 2) emit a radial burst
      if (e->health == 2)
	{
	  const int bullet_count = 10;
	  const double two_pi = 6.28318530717958647692;
	  const double radius = 3.0; // small offset so bullets start outside the sprite
	  int spawned = 0;

	  for (int i = 0; i < MAX_BULLETS && spawned < bullet_count; i++)
	    {
	      if (enemy_bullets[i].damage != 0)
		continue;

	      double theta = (two_pi * spawned) / bullet_count;
	      double dir_x = cos (theta);
	      double dir_y = sin (theta);

	      int vx = (int) round (ENEMY_BULLET_SPEED * dir_x);
	      int vy = (int) round (ENEMY_BULLET_SPEED * dir_y);
	      if (vx == 0 && vy == 0)
		{
		  vy = ENEMY_BULLET_SPEED; // ensure movement if rounding zeroes both components
		}

	      enemy_bullets[i].damage = 2;
	      enemy_bullets[i].velocity.x = vx;
	      enemy_bullets[i].velocity.y = vy;
	      enemy_bullets[i].p.x = e->p.x + (int) round (radius * dir_x);
	      enemy_bullets[i].p.y = e->p.y + (int) round (radius * dir_y);

	      spawned += 1;
	    }
	}
      else
	{
	  for (int i = 0; i < MAX_BULLETS; i++)
	    {
	      // We use damage == 0 to represent an inactive bullet
	      if (enemy_bullets[i].damage == 0)
		{
		  enemy_bullets[i].damage = 1;      // Set damage to activate it
		  enemy_bullets[i].velocity.x = 0;
		  enemy_bullets[i].velocity.y = ENEMY_BULLET_SPEED;
		  enemy_bullets[i].p.x = e->p.x + 1; // Center bullet on the jet (adjust offset as needed)
		  enemy_bullets[i].p.y = e->p.y + 3; // Spawn slightly above the jet

		  break;             // Stop searching after spawning one bullet
		}
	    }
	}

      e->time = current_time_ms;
    }
}

// Use AABB to check whether enemies collide with player bullets
void
update_enemy ()
{
  int bullet_w = PLAYER_BULLET_SPRITE_WIDTH;
  int bullet_h = PLAYER_BULLET_SPRITE_HEIGHT;

  int half_w = ENEMY_SPRITE_WIDTH / 2;
  int min_x = half_w;
  int max_x = IMG_COL - 1 - half_w;
  int min_y = ENEMY_SPRITE_HEIGHT / 2;
  int max_y = ENEMY_SPAWN_RANGE_Y;

  for (int i = 0; i < MAX_ENEMIES; i++)
    {
      enemy *e = &enemies[i];
      if (e->health == 0)
	continue;

      e->p.x += e->velocity.x;
      e->p.y += e->velocity.y;

      if (e->p.x < min_x)
	{
	  e->p.x = min_x;
	  if (e->velocity.x < 0)
	    e->velocity.x = -e->velocity.x;
	}
      else if (e->p.x > max_x)
	{
	  e->p.x = max_x;
	  if (e->velocity.x > 0)
	    e->velocity.x = -e->velocity.x;
	}

      if (e->p.y < min_y)
	{
	  e->p.y = min_y;
	  if (e->velocity.y < 0)
	    e->velocity.y = -e->velocity.y;
	}
      else if (e->p.y > max_y)
	{
	  e->p.y = max_y;
	  if (e->velocity.y > 0)
	    e->velocity.y = -e->velocity.y;
	}

      handle_enemy_shooting (e, (uint64_t) HAL_GetTick ());

      for (int j = 0; j < MAX_PLAYER_BULLETS; j++)
	{
	  bullet *pb = &player_bullets[j];
	  if (pb->damage == 0)
	    continue;

	  if (check_AABB (pb->p.x, pb->p.y, bullet_w, bullet_h, e->p.x, e->p.y,
	  ENEMY_SPRITE_WIDTH,
			  ENEMY_SPRITE_HEIGHT))
	    {
	      e->health -= 1;
	      pb->damage = 0;
	      if (e->health == 0)
		{
		  spawn_enemy_explosion (e->p.x, e->p.y);
		  score += 10;
		  total_enemies -= 1;
		  if (total_enemies <= 0)
		    wave_complete_time = HAL_GetTick ();
		  break;
		}
	    }
	}
    }

  update_enemy_bullets ();
}
