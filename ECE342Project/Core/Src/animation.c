#include <stddef.h>

#include "../Inc/animation.h"
#include "../Inc/sprite.h"

static uint16_t
sanitize_delay (uint16_t frame_delay)
{
  return (frame_delay == 0) ? 1 : frame_delay;
}

void
animation_init (animation *a, const uint8_t *const*frames, uint16_t frame_count,
		uint16_t width_packed, uint16_t width, uint16_t height,
		uint16_t frame_delay, bool loop)
{
  if (!a)
    return;

  a->frames = frames;
  a->frame_count = frame_count;
  a->width_packed = width_packed;
  a->width = width;
  a->height = height;
  a->frame_delay = sanitize_delay (frame_delay);
  a->delay_counter = 0;
  a->current_frame = 0;
  a->loop = loop;
  a->done = (frames == NULL || frame_count == 0);
}

void
animation_reset (animation *a)
{
  if (!a)
    return;

  a->delay_counter = 0;
  a->current_frame = 0;
  a->done = (a->frames == NULL || a->frame_count == 0);
}

void
animation_set_delay (animation *a, uint16_t frame_delay)
{
  if (!a)
    return;

  a->frame_delay = sanitize_delay (frame_delay);
}

bool
animation_finished (const animation *a)
{
  return (!a || a->done);
}

void
animation_play (frame *f, animation *a, int x, int y)
{
  if (!f || !a || !a->frames || a->frame_count == 0 || a->done)
    return;

  const uint8_t *sprite = a->frames[a->current_frame];
  if (sprite)
    {
      draw_sprite (f, x, y, sprite, a->width_packed, a->width, a->height);
    }

  if (a->frame_count == 1)
    return;

  a->delay_counter++;
  if (a->delay_counter >= a->frame_delay)
    {
      a->delay_counter = 0;
      if (a->current_frame + 1 < a->frame_count)
	{
	  a->current_frame++;
	}
      else if (a->loop)
	{
	  a->current_frame = 0;
	}
      else
	{
	  a->done = true;
	}
    }
}
