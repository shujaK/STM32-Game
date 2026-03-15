/*
 * monitor.c
 *
 *  Created on: Mar 15, 2026
 *      Author: shuja
 */

#include "monitor.h"

void
send_frame (frame *f)
{
  // send header
  uint8_t header[HEADER_LEN] =
    { HEADER_BYTE, HEADER_BYTE, HEADER_BYTE, HEADER_BYTE };
  while (CDC_Transmit_FS (header, HEADER_LEN) == USBD_BUSY)
    {
    }

  uint8_t *buf = (uint8_t*) f->data;
  uint32_t remaining = sizeof(f->data);
  uint32_t offset = 0;

  // transmit bytes
  while (remaining > 0)
    {
      uint16_t chunk = (remaining > USB_CHUNK) ? USB_CHUNK : remaining;

      while (CDC_Transmit_FS (&buf[offset], chunk) == USBD_BUSY)
	{
	}

      offset += chunk;
      remaining -= chunk;
    }
}
