#ifndef INC_ENEMY_H_
#define INC_ENEMY_H_

#include "game.h"

extern enemy enemies[MAX_ENEMIES];
extern bullet bullets[MAX_BULLETS];

#define SM_ENEMY_BB 
#define ENEMY_BULLET_SPEED 3

void draw_enemy(frame *f, enemy *e);
void init_test_enemy();
bool check_AABB(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
void update_enemy();

#endif /* INC_ENEMY_H_ */