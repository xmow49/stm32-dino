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
#include "dino/flash.h"

static volatile uint32_t score = 0;
static volatile uint32_t high_score = 0;

static void score_draw();

#define FLASH_ADDR 0x0801FC00

static const uint32_t high_score_ptr = 0x0801FC00;

void score_init()
{
    Flash_Read_Data(high_score_ptr, &high_score, 1);
    if (high_score == 0xFFFFFFFF)
    {
        high_score = 0;
    }
    score_draw();
}

void score_reset()
{
    score = 0;
    score_draw();
}

void score_erase()
{
    score = 1;
    high_score = 0;
    score_save();
    score = 0;
    score_draw();
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

    ILI9341_printf(10, 10, &Font_11x18, COLOR_TEXT_CURRENT, COLOR_SKY_CURRENT, "%lu", score);
    ILI9341_printf(250, 10, &Font_11x18, COLOR_TEXT_CURRENT, COLOR_SKY_CURRENT, "%lu", high_score);
}

void score_save()
{
    if (score <= high_score)
    {
        return;
    }
    high_score = score;

    score_draw();

    Flash_Write_Data(high_score_ptr, &high_score, 1);
    // Verrouiller la mémoire flash
    printf("High score saved: %lu\n\r", high_score);
}
