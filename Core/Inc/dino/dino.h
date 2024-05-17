/*
 * dino.h
 *
 *  Created on: Feb 23, 2024
 *      Author: dbenech
 */

#ifndef INC_DINO_H_
#define INC_DINO_H_

#define COLOR_TEXT_LIGHT 0x0000
#define COLOR_TEXT_DARK 0xffff
#define COLOR_TEXT_CURRENT COLOR_TEXT_LIGHT

#define COLOR_SKY_LIGHT 0x769f
#define COLOR_SKY_DARK 0x8d08
#define COLOR_SKY_CURRENT COLOR_SKY_LIGHT

#define COLOR_GROUND_LIGHT 0xf6b2
#define COLOR_GROUND_DARK 0x0000

#define DINO_GRAVITY_SPEED 5
// #define CACTUS_SPEED 50
#define CACTUS_SPEED 40

#define D_X 82 // DINO INIT X
#define D_Y 124

#define C_X_TARGET 20
#define C_Y_TARGET 140

#define C_X_START 290
#define C_Y_START 140

#define CLOUD_SPEED 1000

int dino_main(); // dino main

#endif /* INC_DINO_H_ */
