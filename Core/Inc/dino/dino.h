/*
 * dino.h
 *
 *  Created on: Feb 23, 2024
 *      Author: dbenech
 */

#ifndef INC_DINO_H_
#define INC_DINO_H_

#include "dino/elements_manager.h"

#define COLOR_TEXT_LIGHT 0x0000
#define COLOR_TEXT_DARK 0xffff
#define COLOR_TEXT_CURRENT elements_manager_get_dark_mode() ? COLOR_TEXT_DARK : COLOR_TEXT_LIGHT

#define COLOR_SKY_LIGHT 0x769f
#define COLOR_SKY_DARK 0x0168
#define COLOR_SKY_CURRENT elements_manager_get_dark_mode() ? COLOR_SKY_DARK : COLOR_SKY_LIGHT

#define COLOR_GROUND_LIGHT 0xf6b2
#define COLOR_GROUND_DARK 0xc52d

#define DINO_GRAVITY_SPEED 5
// #define CACTUS_SPEED 50

extern float ground_speed;

#define CACTUS_SPEED ground_speed // 40

#define D_X 82 // DINO INIT X
#define D_Y 124

#define C_X_START 320
#define C_Y_START 130
#define C_X_TARGET -12
#define C_Y_TARGET 130

#define ROCK_X_START 320
#define ROCK_Y_START 166
#define ROCK_X_TARGET -12
#define ROCK_Y_TARGET 166

#define HOLE_X_START 320
#define HOLE_Y_START 168
#define HOLE_X_TARGET -12
#define HOLE_Y_TARGET 168

#define CLOUD_X_START 320
#define CLOUD_Y_START 43
#define CLOUD_X_TARGET -60
#define CLOUD_Y_TARGET 43

#define CLOUD_SPEED 1000

extern volatile element_id_t current_dino;

int dino_main(); // dino main

#endif /* INC_DINO_H_ */
