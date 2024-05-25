/*
 * sound.c
 *
 *  Created on: 25 May 2024
 *      Author: Dorian
 */

#include "stm32f1xx_hal.h"
#include "main.h"
#include "dino/sound.h"

typedef struct
{
    uint32_t freq;
    uint32_t duration;
} sound_tone_t;

typedef struct
{
    sound_id_t sound_id;
    sound_tone_t tones[10];
} melody_t;

uint32_t stop_time = 0;
sound_id_t current_sound = SOUND_NONE;
uint32_t current_tone = 0;

void sound_end_callback();

// clang-format off
melody_t sounds[] = {
    {SOUND_JUMP, {{1000, 100}}},
    {SOUND_DIE, {{100, 100}, {50, 100}, {25, 100}}},
    {SOUND_HIT, {{100, 100}, {50, 100}, {25, 100}}},
    {SOUND_SCORE, {{700, 200}, {1, 200}, {1200, 200}}},
};
// clang-format on

void sound_set_freq(uint32_t freq)
{
    if (freq < 100)
    {
        return;
    }

    __HAL_TIM_SET_PRESCALER(&htim1, 9);
    uint16_t raw = ((128 * 100000) / freq) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim1, raw);

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = raw / 2;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_Base_Start(&htim1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void sound_stop()
{
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
}

void sound_loop()
{
    if (stop_time != 0 && HAL_GetTick() > stop_time)
    {
        sound_stop();
        stop_time = 0;
        sound_end_callback();
    }
}

void sound_tone(uint32_t freq, uint32_t duration)
{
    if (freq == 0)
    {
        return;
    }
    sound_set_freq(freq);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    stop_time = HAL_GetTick() + duration;
}

melody_t *sound_get_melody(sound_id_t sound)
{
    for (int i = 0; i < sizeof(sounds) / sizeof(melody_t); i++)
    {
        if (sounds[i].sound_id == sound)
        {
            return &sounds[i];
        }
    }
    return NULL;
}

void sound_end_callback()
{
    if (current_sound == SOUND_NONE)
    {
        return;
    }

    melody_t *melody = sound_get_melody(current_sound);
    if (melody == NULL)
    {
        return;
    }

    if (melody->tones[++current_tone].freq == 0)
    {
        current_tone = 0;
        sound_stop();
        return;
    }

    sound_tone(melody->tones[current_tone].freq, melody->tones[current_tone].duration);
}

void sound_play(sound_id_t sound)
{
    melody_t *melody = sound_get_melody(sound);
    if (melody == NULL)
    {
        return;
    }
    current_tone = 0;
    current_sound = sound;
    sound_tone(melody->tones[current_tone].freq, melody->tones[current_tone].duration);
}
