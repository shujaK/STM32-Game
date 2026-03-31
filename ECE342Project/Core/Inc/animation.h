#ifndef INC_ANIMATION_H_
#define INC_ANIMATION_H_

#include <stdbool.h>
#include <stdint.h>

#include "monitor.h"

typedef struct animation
{
  const uint8_t *const*frames;
  uint16_t frame_count;
  uint16_t width;
  uint16_t width_packed;
  uint16_t height;
  uint16_t frame_delay;
  uint16_t delay_counter;
  uint16_t current_frame;
  bool loop;
  bool done;
} animation;

void
animation_init (animation *a, const uint8_t *const*frames, uint16_t frame_count,
		uint16_t width_packed, uint16_t width, uint16_t height,
		uint16_t frame_delay, bool loop);

void
animation_play (frame *f, animation *a, int x, int y);

void
animation_reset (animation *a);

void
animation_set_delay (animation *a, uint16_t frame_delay);

bool
animation_finished (const animation *a);

#endif /* INC_ANIMATION_H_ */
