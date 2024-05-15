/*
 * collision.c
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

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
bool collision_check()
{

    element_t *dino = elements_manager_find_element(ID_DINO);
    element_t *cactus = elements_manager_find_element(ID_CACTUS_1);

    if ((dino->x + (dino->width * dino->data.sprite.scale) > cactus->x) &&
        (dino->x < cactus->x + (cactus->width * cactus->data.sprite.scale)) &&
        (dino->y + (dino->height * dino->data.sprite.scale) > cactus->y) &&
        (dino->y < cactus->y + (cactus->height * cactus->data.sprite.scale)))
    {
        cactus->move.status = MOVE_NO;
        dino->move.status = MOVE_NO;
        return true;
    }

    return false;
}
