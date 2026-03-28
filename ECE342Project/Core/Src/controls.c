/*
 * controls.c
 *
 *  Created on: Mar 19, 2026
 *  Author: Hardy L.
 *
 * Description: Create a structure that used to update the movement of the plane every frame
 */

#include "controls.h"
#include "main.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;

volatile bool flag_shoot = false;
volatile bool flag_bomb = false;
void
update_controls (controls *c)
{
  c->button_shoot = flag_shoot;

  c->button_bomb = flag_bomb;

  flag_shoot = false;
  flag_bomb = false;

  HAL_ADC_Start (&hadc1);
  HAL_ADC_PollForConversion (&hadc1, 10);
  uint32_t raw_x = HAL_ADC_GetValue (&hadc1);
  HAL_ADC_PollForConversion (&hadc1, 10);
  uint32_t raw_y = HAL_ADC_GetValue (&hadc1);
  HAL_ADC_Stop (&hadc1);

  float norm_x = ((float) raw_x - 2048.0f) / 2047.0f;
  float norm_y = ((float) raw_y - 2048.0f) / 2047.0f;

  if (norm_x > -0.05f && norm_x < 0.05f)
    norm_x = 0.0f;
  if (norm_y > -0.05f && norm_y < 0.05f)
    norm_y = 0.0f;

  if (norm_x > 1.0f)
    norm_x = 1.0f;
  else if (norm_x < -1.0f)
    norm_x = -1.0f;

  if (norm_y > 1.0f)
    norm_y = 1.0f;
  else if (norm_y < -1.0f)
    norm_y = -1.0f;

  c->joystick_x = norm_x;
  c->joystick_y = norm_y;
}
