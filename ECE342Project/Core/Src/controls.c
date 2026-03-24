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
#include <stdio.h>
#include <string.h>

extern ADC_HandleTypeDef hadc1;

// Volatile flags updated by the hardware interrupts
volatile bool flag_shoot = false;
volatile bool flag_bomb = false;

// Interrupt Handler
// This overrides the weak HAL function. It runs instantly when a button is pressed.
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
// Check which pin triggered the interrupt
// if (GPIO_Pin == GPIO_PIN_15) {        // PF15 -> Fire 1
//     flag_shoot = true;
// }
// else if (GPIO_Pin == GPIO_PIN_14) {   // PF14 -> Fire 2
//     flag_bomb = true;
// }
// else if (GPIO_Pin == GPIO_PIN_13) {   // PF13 -> SW Joystick Click
//     // You can add a pause_menu flag here later!
// }
// }

/* -------------------------------------------------------------------------- */
/* 2. THE FRAME UPDATER (Game Loop Level)                                     */
/* -------------------------------------------------------------------------- */
void update_controls(controls *c)
{
    char buff[100];
    // Transfer the hardware flags into your game struct
    c->button_shoot = flag_shoot;

    c->button_bomb = flag_bomb;

    if (flag_shoot)
    {
        // print_msg("Shoot Pressed\n");
    }

    if (flag_bomb)
    {
        // print_msg("Bomb Pressed\n");
    }

    // Reset the flags so the weapon doesn't fire infinitely
    flag_shoot = false;
    flag_bomb = false;

    // --- JOYSTICK (ADC POLLING) ---
    // Start the ADC
    HAL_ADC_Start(&hadc1);

    // Wait for Rank 1 (X-Axis) and read it
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint32_t raw_x = HAL_ADC_GetValue(&hadc1);

    // Wait for Rank 2 (Y-Axis) and read it
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint32_t raw_y = HAL_ADC_GetValue(&hadc1);

    // sprintf(buff, "X: %d, Y: %d\n", raw_x, raw_y);
    // print_msg(buff);
    // Stop the ADC
    HAL_ADC_Stop(&hadc1);

    // --- JOYSTICK MATH (Normalization & Deadzone) ---
    // The ADC reads 0 to 4095. Center is roughly 2048.
    // We want to normalize this to a float between -1.0f and 1.0f
    float norm_x = (raw_x - 2002.0f) / 2002.0f;
    float norm_y = (raw_y - 2040.0f) / 2040.0f;

    // Apply a 15% deadzone to stop the plane from drifting when you let go
    if (norm_x > -0.05f && norm_x < 0.05f)
        norm_x = 0.0f;
    if (norm_y > -0.05f && norm_y < 0.05f)
        norm_y = 0.0f;

    // norm_x *= -1.0f;
    c->joystick_x = norm_x;
    c->joystick_y = norm_y;

    // sprintf(buff, "X: %.5f, Y: %.5f\n", norm_x, norm_y);
    // print_msg(buff);
}