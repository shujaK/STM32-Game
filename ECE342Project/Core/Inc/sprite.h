/*
 * sprite.h
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#ifndef INC_SPRITE_H_
#define INC_SPRITE_H_

#include "monitor.h"

// 5x5 diamond, packed with w_packed=6 (padded to even), WHITE=0x1, BLACK=0x0
static const uint8_t jet_sprite[15] =
  { 0x00, 0x10, 0x00,  // row 0: . . W . . _
      0x01, 0x11, 0x00,  // row 1: . W W W . _
      0x11, 0x11, 0x10,  // row 2: W W W W W _
      0x01, 0x11, 0x00,  // row 3: . W W W . _
      0x00, 0x10, 0x00,  // row 4: . . W . . _
    };

// sprite: packed pix2 data
// w_packed: width in pixels rounded up to even (for packing), must be even
// w, h: true sprite dimensions
// x, y are the centre pixel of the sprite
static void
draw_sprite (frame *f, uint16_t x, uint16_t y, const uint8_t *sprite,
	     uint16_t w_packed, uint16_t w, uint16_t h)
{
  uint16_t bytes_per_row = w_packed / 2;
  uint16_t ox = x - (w - 1) / 2;
  uint16_t oy = y - (h - 1) / 2;
  for (int i = 0; i < bytes_per_row * h; i++)
    {
      uint8_t b = sprite[i];
      uint8_t p1 = b >> 4;
      uint8_t p2 = b & 0x0F;
      uint16_t col = (i % bytes_per_row) * 2;
      uint16_t row = (i / bytes_per_row);
      if (col < w)
	write_pixel (f, ox + col, oy + row, p1);
      if (col + 1 < w)
	write_pixel (f, ox + col + 1, oy + row, p2);
    }
}
S
#endif /* INC_SPRITE_H_ */
