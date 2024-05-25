/*
 * sound.h
 *
 *  Created on: 25 May 2024
 *      Author: Dorian
 */

#ifndef INC_DINO_SOUND_H_
#define INC_DINO_SOUND_H_

#include "stm32f1xx_hal.h"

typedef enum
{
    SOUND_NONE,
    SOUND_JUMP,
    SOUND_DIE,
    SOUND_HIT,
    SOUND_SCORE
} sound_id_t;

void sound_loop();
void sound_stop();

void sound_tone(uint32_t freq, uint32_t duration);
void sound_play(sound_id_t sound);

#endif /* INC_DINO_SOUND_H_ */
