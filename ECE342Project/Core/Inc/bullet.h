#ifndef INC_BULLET_H_
#define INC_BULLET_H_

#include "game.h"

void draw_bullet(frame *f, bullet *b);
void draw_player_bullet(frame *f, bullet *b);
void draw_enemy_bullet(frame *f, bullet *b);
void init_bullets();
void update_bullets();

#endif /* INC_BULLET_H_ */