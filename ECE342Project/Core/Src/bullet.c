#include <stdbool.h>

#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "sprite.h"
#include "animation.h"

#define MAX_BULLET_EXPLOSIONS MAX_BULLETS
#define BULLET_EXPLOSION_FRAME_DELAY 2

typedef struct bullet_explosion_instance
{
  animation anim;
  int x;
  int y;
  bool active;
} bullet_explosion_instance;

static const uint8_t *bullet_explosion_frames[EXPLOSION_SPRITE_COUNT] =
  { explosion[0], explosion[1], explosion[2], explosion[3], explosion[4] };

static bullet_explosion_instance bullet_explosions[MAX_BULLET_EXPLOSIONS];

static void
reset_bullet_explosion (bullet_explosion_instance *e)
{
  if (!e)
    return;
  e->active = false;
}

void
draw_bullet (frame *f, bullet *b)
{
  const uint8_t *s;
  if (b->damage == 1)
    s = &enemy_bullet_sprite;
  if (b->damage == 2)
    s = &enemy_bullet_sprite_2;
  draw_sprite (f, b->p.x, b->p.y, s, ENEMY_BULLET_SPRITE_WIDTH_PACKED,
  ENEMY_BULLET_SPRITE_WIDTH,
	       ENEMY_BULLET_SPRITE_HEIGHT);
}

void
draw_player_bullet (frame *f, bullet *b)
{
  draw_sprite (f, b->p.x, b->p.y, player_bullet_sprite,
  PLAYER_BULLET_SPRITE_WIDTH_PACKED,
	       PLAYER_BULLET_SPRITE_WIDTH, PLAYER_BULLET_SPRITE_HEIGHT);
}

void
init_bullets ()
{
  for (int i = 0; i < MAX_BULLETS; i++)
    {
      enemy_bullets[i].damage = 0;
      enemy_bullets[i].velocity.x = 0;
      enemy_bullets[i].velocity.y = 0;
    }

  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
    {
      player_bullets[i].damage = 0;
      player_bullets[i].velocity.x = 0;
      player_bullets[i].velocity.y = 0;
    }

  for (int i = 0; i < MAX_BULLET_EXPLOSIONS; i++)
    {
      reset_bullet_explosion (&bullet_explosions[i]);
    }
}

void
draw_enemy_bullet (frame *f, bullet *b)
{
  draw_sprite (f, b->p.x, b->p.y, enemy_bullet_sprite,
  ENEMY_BULLET_SPRITE_WIDTH_PACKED,
	       ENEMY_BULLET_SPRITE_WIDTH, ENEMY_BULLET_SPRITE_HEIGHT);
}

void
update_bullets ()
{
  for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
    {
      if (player_bullets[i].damage > 0)
	{
	  player_bullets[i].p.x += player_bullets[i].velocity.x;
    player_bullets[i].p.y += player_bullets[i].velocity.y;

	  if (player_bullets[i].p.y
	      < 3|| player_bullets[i].p.x < 0 || player_bullets[i].p.x > IMG_COL)
	    {
	      player_bullets[i].damage = 0;
	    }
	}
    }
}

void
spawn_bullet_explosion (int x, int y)
{
  for (int i = 0; i < MAX_BULLET_EXPLOSIONS; i++)
    {
      bullet_explosion_instance *slot = &bullet_explosions[i];
      if (slot->active)
	continue;

      animation_init (&slot->anim, bullet_explosion_frames,
      EXPLOSION_SPRITE_COUNT,
		      EXPLOSION_SPRITE_WIDTH_PACKED,
		      EXPLOSION_SPRITE_WIDTH,
		      EXPLOSION_SPRITE_HEIGHT,
		      BULLET_EXPLOSION_FRAME_DELAY,
		      false);
      slot->x = x;
      slot->y = y;
      slot->active = true;
      return;
    }
}

void
draw_bullet_explosions (frame *f)
{
  if (!f)
    return;

  for (int i = 0; i < MAX_BULLET_EXPLOSIONS; i++)
    {
      bullet_explosion_instance *slot = &bullet_explosions[i];
      if (!slot->active)
	continue;

      animation_play (f, &slot->anim, slot->x, slot->y);
      if (animation_finished (&slot->anim))
	{
	  reset_bullet_explosion (slot);
	}
    }
}
