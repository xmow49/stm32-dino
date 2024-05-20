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

    element_t *dino = elements_manager_find_element(current_dino);
    element_t *cactus = elements_manager_find_element(ID_CACTUS_1);

    if ((dino->x + (dino->width * dino->data.sprite.scale) > cactus->x) &&
        (dino->x < cactus->x + (cactus->width * cactus->data.sprite.scale)) &&
        (dino->y + (dino->height * dino->data.sprite.scale) > cactus->y) &&
        (dino->y < cactus->y + (cactus->height * cactus->data.sprite.scale)))
    {
        // check every pixel, check the pixel position is the same (x,y) for both elements and ignore it if it's transparent(0x0000)
        for (int y_dino = 0; y_dino < dino->height * dino->data.sprite.scale; y_dino++)
        {
            for (int x_dino = 0; x_dino < dino->width * dino->data.sprite.scale; x_dino++)
            {
                uint16_t dino_pixel = dino->data.sprite.sprite[(x_dino/2) + ((y_dino/2) * dino->width)];
                if (dino_pixel != 0x0000)
                {
                    for (int y_cactus = 0; y_cactus < cactus->height * cactus->data.sprite.scale; y_cactus++)
                    {
                        for (int x_cactus = 0; x_cactus < cactus->width * cactus->data.sprite.scale; x_cactus++)
                        {
                            uint16_t cactus_pixel = cactus->data.sprite.sprite[x_cactus/2 + (y_cactus/2) * cactus->width];
                            if (cactus_pixel != 0x0000)
                            {
                                if (dino->x + x_dino == cactus->x + x_cactus && dino->y + y_dino == cactus->y + y_cactus)
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}
