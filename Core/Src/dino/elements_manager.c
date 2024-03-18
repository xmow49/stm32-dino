
#include <stdint.h>

#include "dino/elements_manager.h"
#include "dino/sprites.h"
#include "dino/dino.h"
#include "dino/framebuffer.h"
#include "dino/move_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <lib/tft_ili9341/stm32f1_ili9341.h>

// clang-format off
element_t elements_list[] = {
    {TYPE_SPRITE,   ID_DINO,        82,  124,    20, 22,  .data.sprite = {2, sprite_dino_stand}, MOVE_NO},
//    {TYPE_SPRITE,   ID_CACTUS_0,   151,  132,    9,  19,  .data.sprite = {2, sprite_cactus}, MOVE_NO},
    {TYPE_SPRITE,   ID_CACTUS_1,   200,  140,    9,  19,  .data.sprite = {2, sprite_cactus}, MOVE_NO},
    // {TYPE_SPRITE,   ID_GAME_OVER,   48,  53,    73,  9,  .data.sprite = {3, sprite_gameover}, MOVE_NO},
//    {TYPE_BITMAP,   ID_COPYRIGHT,   31,  175,   129,  26,  .data.sprite = {2, sprite_copyright}, MOVE_NO},
    {TYPE_FILL,     ID_SKY,         0,  0,     320, 168,   .data.fill = {COLOR_SKY_LIGHT},      MOVE_NO},
    {TYPE_FILL,     ID_GROUND,      0,  169,   320, 72,    .data.fill = {COLOR_GROUND_LIGHT},   MOVE_NO},
};

const uint16_t elements_count = sizeof(elements_list) / sizeof(element_t);

// clang-format on

uint16_t framebuffer[32 * 176];

int elements_manager_update_full_screen()
{
    for (int i = elements_count - 1; i >= 0; i--)
    {
        const element_t *element = &elements_list[i];
        switch (element->type)
        {
        case TYPE_FILL:
            ILI9341_DrawFilledRectangle(element->x, element->y, element->x + element->width, element->y + element->height, element->data.fill.color);
            break;
        case TYPE_SPRITE:
            fb_generate_background(framebuffer, element->x, element->y + 1, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale);
            fb_draw_bitmap(framebuffer, 0, 0, element->width, element->height, element->data.sprite.sprite, element->data.sprite.scale, element->width * element->data.sprite.scale);
            ILI9341_putBitmap(element->x, element->y, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale, 1, framebuffer, element->width * element->data.sprite.scale * element->height * element->data.sprite.scale);
            break;
        case TYPE_BITMAP:
            fb_generate_background(framebuffer, element->x, element->y + 1, element->width, element->height);
            //            fb_draw_bitmap(framebuffer, 0, 0, element->width, element->height, element->data.sprite.scale, 1, element->width);
            ILI9341_putBitmap(element->x, element->y, element->width, element->height, element->data.sprite.sprite, framebuffer, element->width * element->data.sprite.scale * element->height * element->data.sprite.scale);
            break;
            break;
        }
    }
    return 0;
}

element_t *elements_manager_find_element(element_id_t id)
{
    for (int i = 0; i < elements_count; i++)
    {
        if (elements_list[i].id == id)
        {
            return &elements_list[i];
        }
    }
    return NULL;
}

void fb_generate_background(uint16_t *framebuffer, uint16_t framebuffer_x_offset, uint16_t framebuffer_y_offset, uint16_t width, uint16_t height)
{
    int32_t framebuffer_index = width * height - 1;
    for (int j = elements_count - 1; j >= 0; j--)
    {
        const element_t *bg_element = &elements_list[j];
        uint16_t bg_max_x = bg_element->x + bg_element->width;
        uint16_t bg_max_y = bg_element->y + bg_element->height;
        if (bg_element->type != TYPE_FILL)
        {
            continue;
        }
        for (int y = framebuffer_y_offset; y < framebuffer_y_offset + (height); y++)
        {
            for (int x = framebuffer_x_offset; x < framebuffer_x_offset + (width); x++)
            {
                // framebuffer[framebuffer_index] = bg_element->data.fill.color;
                if (x >= bg_element->x && x <= bg_max_x && y >= bg_element->y && y <= bg_max_y)
                {
                    if (framebuffer_index >= 0)
                    {
                        framebuffer[framebuffer_index] = bg_element->data.fill.color;
                        framebuffer_index--;
                    }
                }
            }
        }
    }
}

int elements_manager_move_element(element_id_t id, int16_t target_x, int16_t target_y)
{
    element_t *element = elements_manager_find_element(id);
    if (!element)
    {
        return -1;
    }
    if (element->type == TYPE_FILL)
    {
        return -1;
    }

    if (target_x < 0 || target_y < 0)
    {
        return -1;
    }

    if (target_x + element->width * element->data.sprite.scale >= SCREEN_WIDTH)
    {
        return -1;
    }

    if (target_y + element->height * element->data.sprite.scale >= SCREEN_HEIGHT)
    {
        return -1;
    }

    // check if the new values are overlap the last position

    if (abs(element->x - target_x) < element->width * element->data.sprite.scale && abs(element->y - target_y) < element->height * element->data.sprite.scale)
    {
        uint16_t fb_width = element->width * element->data.sprite.scale + abs(element->x - target_x);
        uint16_t fb_height = element->height * element->data.sprite.scale + abs(element->y - target_y);

        uint16_t bitmap_x = 0;
        uint16_t bitmap_y = 0;

        fb_generate_background(framebuffer, element->x, element->y + 1, fb_width, fb_height);
        if ((int32_t)element->x - (int32_t)target_x < 0)
        {
            bitmap_x = fb_width - (element->width * element->data.sprite.scale);
        }
        else
        {
            bitmap_x = 0;
        }

        if ((int32_t)element->y - (int32_t)target_y < 0)
        {
            bitmap_y = fb_height - (element->height * element->data.sprite.scale);
        }
        else
        {
            bitmap_y = 0;
        }
        fb_draw_bitmap(framebuffer, bitmap_x, bitmap_y, element->width, element->height, element->data.sprite.sprite, element->data.sprite.scale, fb_width);
        ILI9341_putBitmap(element->x, element->y, fb_width, fb_height, 1, framebuffer, fb_width * fb_height);
        element->x = target_x;
        element->y = target_y;
        return 0;
    }

    fb_generate_background(framebuffer, element->x, element->y + 1, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale);
    ILI9341_putBitmap(element->x, element->y, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale, 1, framebuffer, element->width * element->data.sprite.scale * element->height * element->data.sprite.scale);

    element->x = target_x;
    element->y = target_y;

    fb_generate_background(framebuffer, element->x, element->y + 1, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale);
    fb_draw_bitmap(framebuffer, 0, 0, element->width, element->height, element->data.sprite.sprite, element->data.sprite.scale, element->width * element->data.sprite.scale);
    ILI9341_putBitmap(element->x, element->y, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale, 1, framebuffer, element->width * element->data.sprite.scale * element->height * element->data.sprite.scale);

    return 0;
}
