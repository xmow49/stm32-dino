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
#include "dino/enemy.h"

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
bool collision_check()
{

    element_t *dino = elements_manager_find_element(current_dino);
    element_t *enemy = elements_manager_find_element(enemy_get_current());
    int dino_right = dino->x + (dino->width * dino->data.sprite.scale);
    int enemy_right = enemy->x + (enemy->width * enemy->data.sprite.scale);
    int dino_bottom = dino->y + (dino->height * dino->data.sprite.scale);
    int enemy_bottom = enemy->y + (enemy->height * enemy->data.sprite.scale);

    if (dino_right > enemy->x && dino->x < enemy_right && dino_bottom > enemy->y && dino->y < enemy_bottom)
    {

        int overlap_left = (dino->x > enemy->x) ? dino->x : enemy->x;
        int overlap_right = (dino_right < enemy_right) ? dino_right : enemy_right;
        int overlap_top = (dino->y > enemy->y) ? dino->y : enemy->y;
        int overlap_bottom = (dino_bottom < enemy_bottom) ? dino_bottom : enemy_bottom;

        for (int y = overlap_top; y < overlap_bottom; y++)
        {
            for (int x = overlap_left; x < overlap_right; x++)
            {
                int dino_x = (x - dino->x) / dino->data.sprite.scale;
                int dino_y = (y - dino->y) / dino->data.sprite.scale;
                uint16_t dino_pixel = dino->data.sprite.sprite[dino_x + dino_y * dino->width];

                int cactus_x = (x - enemy->x) / enemy->data.sprite.scale;
                int cactus_y = (y - enemy->y) / enemy->data.sprite.scale;
                uint16_t cactus_pixel = enemy->data.sprite.sprite[cactus_x + cactus_y * enemy->width];

                if (dino_pixel != 0x0000 && cactus_pixel != 0x0000)
                {
                    return true;
                }
            }
        }
    }

    return false;
}
