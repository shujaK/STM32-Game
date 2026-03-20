/*
 * controls.h
 *
 *  Created on: Mar 19, 2026
 *  Author: Hardy L.
 * 
 * Description: Create a structure that used to update the movement of the plane every frame
 */

#ifndef INC_CONTROLS_H_
#define INC_CONTROLS_H_

#include <stdbool.h>

typedef struct 
{
    bool button_shoot;
    bool button_bomb;
    float joystick_x;
    float joystick_y;
}controls;

void update_controls(controls* current_controls);

#endif /* INC_CONTROLS_H_ */