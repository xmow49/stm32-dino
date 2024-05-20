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
    int dino_right = dino->x + (dino->width * dino->data.sprite.scale);
    int cactus_right = cactus->x + (cactus->width * cactus->data.sprite.scale);
    int dino_bottom = dino->y + (dino->height * dino->data.sprite.scale);
    int cactus_bottom = cactus->y + (cactus->height * cactus->data.sprite.scale);

    // if ((dino->x + (dino->width * dino->data.sprite.scale) > cactus->x) &&
    //     (dino->x < cactus->x + (cactus->width * cactus->data.sprite.scale)) &&
    //     (dino->y + (dino->height * dino->data.sprite.scale) > cactus->y) &&
    //     (dino->y < cactus->y + (cactus->height * cactus->data.sprite.scale)))
    // {
    if (dino_right > cactus->x && dino->x < cactus_right && dino_bottom > cactus->y && dino->y < cactus_bottom)
    {

        int overlap_left = (dino->x > cactus->x) ? dino->x : cactus->x;
        int overlap_right = (dino_right < cactus_right) ? dino_right : cactus_right;
        int overlap_top = (dino->y > cactus->y) ? dino->y : cactus->y;
        int overlap_bottom = (dino_bottom < cactus_bottom) ? dino_bottom : cactus_bottom;

        for (int y = overlap_top; y < overlap_bottom; y++)
        {
            for (int x = overlap_left; x < overlap_right; x++)
            {
                int dino_x = (x - dino->x) / dino->data.sprite.scale;
                int dino_y = (y - dino->y) / dino->data.sprite.scale;
                uint16_t dino_pixel = dino->data.sprite.sprite[dino_x + dino_y * dino->width];

                int cactus_x = (x - cactus->x) / cactus->data.sprite.scale;
                int cactus_y = (y - cactus->y) / cactus->data.sprite.scale;
                uint16_t cactus_pixel = cactus->data.sprite.sprite[cactus_x + cactus_y * cactus->width];

                if (dino_pixel != 0x0000 && cactus_pixel != 0x0000)
                {
                    return true;
                }
            }
        }

        // check every pixel, check the pixel position is the same (x,y) for both elements and ignore it if it's transparent(0x0000)
        // for (int y_dino = 0; y_dino < dino->height * dino->data.sprite.scale; y_dino++)
        // {
        //     for (int x_dino = 0; x_dino < dino->width * dino->data.sprite.scale; x_dino++)
        //     {
        //         uint16_t dino_pixel = dino->data.sprite.sprite[(x_dino / 2) + ((y_dino / 2) * dino->width)];
        //         if (dino_pixel != 0x0000)
        //         {
        //             for (int y_cactus = 0; y_cactus < cactus->height * cactus->data.sprite.scale; y_cactus++)
        //             {
        //                 for (int x_cactus = 0; x_cactus < cactus->width * cactus->data.sprite.scale; x_cactus++)
        //                 {
        //                     uint16_t cactus_pixel = cactus->data.sprite.sprite[x_cactus / 2 + (y_cactus / 2) * cactus->width];
        //                     if (cactus_pixel != 0x0000)
        //                     {
        //                         if (dino->x + x_dino == cactus->x + x_cactus && dino->y + y_dino == cactus->y + y_cactus)
        //                         {
        //                             return true;
        //                         }
        //                     }
        //                 }
        //             }
        //         }
        //     }
        // }
    }

    return false;
}
