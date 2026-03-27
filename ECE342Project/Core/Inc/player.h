#ifndef INC_PLAYER_H_
#define INC_PLAYER_H_

#include "game.h"

extern player player1;
extern bullet player_bullets[MAX_PLAYER_BULLETS];

void draw_player(frame *f, player *p);
void update_player(player *p, controls *c);
void handle_shooting(player *p, controls *c, uint32_t current_time_ms);

#endif /* INC_PLAYER_H_ */