#ifndef INC_ANIMATION_H_
#define INC_ANIMATION_H_

#include <stdint.h>
#include <stdbool.h>
#include "game.h"
#include "monitor.h"

#define MAX_EXPLOSIONS 10
#define EXPLOSION_FRAMES 6
#define ANIMATION_SPEED_MS 50

typedef struct
{
  int2d p;
  uint8_t current_frame;
  bool active;
  uint32_t last_update;
} explosion_anim;
extern explosion_anim explosions[MAX_EXPLOSIONS];

void init_animations();
void spawn_explosion(int x, int y, uint32_t current_time_ms);
void update_animations(uint32_t current_time_ms);
void draw_animations(frame *f);

#endif /* INC_ANIMATION_H_ */