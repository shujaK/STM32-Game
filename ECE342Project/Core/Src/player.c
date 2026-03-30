#include "player.h"
#include "sprite.h"
#include "enemy.h"

player player1;
bullet player_bullets[MAX_PLAYER_BULLETS];
uint16_t last_fire_time = 0;
uint32_t last_special_time = 0;
uint16_t last_damage_taken_time = 0;

void
draw_player (frame *f, player *p)
{
  if (p->velocity.x > 0.7)
    {
      draw_sprite (f, p->p.x, p->p.y, player_right_sprite,
      PLAYER_SPRITE_WIDTH_PACKED,
		   PLAYER_SPRITE_WIDTH,
		   PLAYER_SPRITE_HEIGHT);
    }
  else if (p->velocity.x < -0.7)
    {
      draw_sprite (f, p->p.x, p->p.y, player_left_sprite,
      PLAYER_SPRITE_WIDTH_PACKED,
		   PLAYER_SPRITE_WIDTH,
		   PLAYER_SPRITE_HEIGHT);
    }
  else
    {
      draw_sprite (f, p->p.x, p->p.y, player_sprite, PLAYER_SPRITE_WIDTH_PACKED,
      PLAYER_SPRITE_WIDTH,
		   PLAYER_SPRITE_HEIGHT);
    }

  if (p->velocity.y < -0.5)
    {
      draw_sprite (f, p->p.x, p->p.y + 12, booster_sprite,
      BOOSTER_SPRITE_WIDTH_PACKED,
		   BOOSTER_SPRITE_WIDTH,
		   BOOSTER_SPRITE_HEIGHT);
    }
}

// Pass in your current system time (e.g., HAL_GetTick())
// LATER: CREATE ANOTHER ARRAY FOR PLAYER BULLETS
void
handle_shooting (player *p, controls *c, uint64_t current_time_ms)
{
  if (c->button_shoot)
    {
      // Check if 200ms have passed since the last shot
      if ((current_time_ms - last_fire_time) >= FIRE_COOLDOWN_MS)
	{
	  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
	    {
	      // We use damage == 0 to represent an inactive bullet
	      if (player_bullets[i].damage == 0)
		{
		  player_bullets[i].damage = 1;     // Set damage to activate it
		  player_bullets[i].velocity.x = 0;
		  player_bullets[i].velocity.y = -PLAYER_BULLET_SPEED; // upward motion
		  player_bullets[i].p.x = p->p.x + 1; // Center bullet on the jet (adjust offset as needed)
		  player_bullets[i].p.y = p->p.y - 3; // Spawn slightly above the jet

		  last_fire_time = current_time_ms;
		  break;             // Stop searching after spawning one bullet
		}
	    }
	}
    }

  if (current_time_ms - last_special_time >= SPECIAL_COOLDOWN_MS)
    {
      p->special_available = true;
      if (c->button_bomb)
	{
	  for (int i = 0; i < MAX_BULLETS; i++)
	  	{
	  	  enemy_bullets[i].damage = 0;
	  	  last_special_time = current_time_ms;
	  	}
	}
    }
  else
    {
      p->special_available = false;
    }

}

void
update_player (player *p, controls *c)
{
  p->velocity.x = c->joystick_x * 3; // scale the joystick input to control speed
  p->velocity.y = c->joystick_y * 5;
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

  uint64_t current_time_ms = (uint64_t) HAL_GetTick ();
  handle_shooting (p, c, current_time_ms);

  // loop over all enemy bullets and check for collisions with the player, if so
  // reduce health by 1 and remove the bullet (set damage to 0)
  for (int i = 0; i < MAX_BULLETS; i++)
    {
      if (enemy_bullets[i].damage > 0)
	{
	  // Check collision with player using AABB
	  if (check_AABB (p->p.x - PLAYER_SPRITE_WIDTH / 2,
			  p->p.y - PLAYER_SPRITE_HEIGHT / 2,
			  PLAYER_SPRITE_WIDTH,
			  PLAYER_SPRITE_HEIGHT,
			  enemy_bullets[i].p.x - ENEMY_BULLET_SPRITE_WIDTH / 2,
			  enemy_bullets[i].p.y - ENEMY_BULLET_SPRITE_HEIGHT / 2,
			  ENEMY_BULLET_SPRITE_WIDTH,
			  ENEMY_BULLET_SPRITE_HEIGHT))
	    {
	      uint32_t curr_time = HAL_GetTick ();
	      if (p->health > 0
		  && (curr_time - last_damage_taken_time > INVULNERABILITY_TIME))
		{
		  p->health -= 1;
		  enemy_bullets[i].damage = 0;
		  last_damage_taken_time = curr_time;
		  if (p->health < 1)
		    {
		      game_start = false; // end game when player is out of health
		    }
		}
	    }
	}
    }
}
