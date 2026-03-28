#include "player.h"
#include "sprite.h"

player player1;
bullet player_bullets[MAX_PLAYER_BULLETS];
uint16_t last_fire_time = 0;

void draw_player(frame *f, player *p)
{
  // draw_sprite(f, p->p.x, p->p.y, jet_sprite, 6, 5, 5);
  draw_sprite(f, p->p.x, p->p.y, advanced_jet_sprite, 8, 7, 7);
}

// Pass in your current system time (e.g., HAL_GetTick())
// LATER: CREATE ANOTHER ARRAY FOR PLAYER BULLETS
void handle_shooting(player *p, controls *c, uint64_t current_time_ms)
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
          player_bullets[i].damage = 1;           // Set damage to activate it
          player_bullets[i].speed = PLAYER_BULLET_SPEED; 
          player_bullets[i].p.x = p->p.x + 1;     // Center bullet on the jet (adjust offset as needed)
          player_bullets[i].p.y = p->p.y - 3;     // Spawn slightly above the jet

          last_fire_time = current_time_ms; 
          break;                            // Stop searching after spawning one bullet
        }
      }
    }
  }
}

void update_player(player *p, controls *c)
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

  uint64_t current_time_ms = (uint64_t)HAL_GetTick();
  handle_shooting(p, c, current_time_ms);
}
