#ifndef INC_BULLET_H_
#define INC_BULLET_H_

#include "game.h"

typedef struct bullet
{
  int2d p;
  uint8_t damage;
  int2d velocity;
} bullet;

void
draw_bullet (frame *f, bullet *b);
void
draw_player_bullet (frame *f, bullet *b);
void
draw_enemy_bullet (frame *f, bullet *b);
void
init_bullets ();
void
update_bullets ();

#endif /* INC_BULLET_H_ */
