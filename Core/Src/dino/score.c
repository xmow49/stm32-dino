/*
 * score.c
 *
 *  Created on: Apr 26, 2024
 *      Author: dbenech
 */
#include "stm32f1xx_hal.h"
#include "lib/stm/stm32f1_uart.h"
#include "macro_types.h"
#include "lib/stm/stm32f1_gpio.h"
#include <stdint.h>
#include <lib/tft_ili9341/stm32f1_ili9341.h>

#include "dino/dino.h"
#include "dino/sprites.h"
#include "dino/framebuffer.h"
#include "dino/elements_manager.h"
#include "dino/move_manager.h"

static uint32_t score = 0;

static void score_draw();

void score_init()
{
}

void score_reset()
{
}

void score_update()
{
    score++;
    if (score % 10 == 0)
    {
        score_draw();
    }
}

static void score_draw()
{

    ILI9341_printf(10, 10, &Font_11x18, COLOR_TEXT_LIGHT, COLOR_SKY_CURRENT, "%d", score);
}
