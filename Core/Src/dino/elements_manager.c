
#include <stdint.h>

#include "dino/elements_manager.h"
#include "dino/sprites.h"
#include "dino/dino.h"
#include "dino/framebuffer.h"
#include "dino/move_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <lib/tft_ili9341/stm32f1_ili9341.h>

// clang-format off
element_t elements_list[] = {
    {TYPE_SPRITE,   ID_DINO,       D_X,  D_Y,   20,  22,  .data.sprite = {2, sprite_dino_stand}, { MOVE_NO }, true},

    {TYPE_SPRITE,   ID_CACTUS_1,   C_X,  C_Y,    9,  19,  .data.sprite = {2, sprite_cactus},     { MOVE_NO }, true},

    {TYPE_SPRITE,   ID_GAME_OVER,   48,  53,    73,   9,  .data.sprite = {3, sprite_gameover},   { MOVE_NO }, false}, //5913
    {TYPE_BITMAP,   ID_COPYRIGHT,   31,  178,  129,  26,  .data.sprite = {2, sprite_copyright},  { MOVE_NO }, true},

    // {TYPE_SPRITE,   ID_CLOUD_0,     57,  43,    28,  13,  .data.sprite = {2, sprite_cloud},      { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_CLOUD_1,    227,  43,    28,  13,  .data.sprite = {2, sprite_cloud},      { MOVE_NO }, true},
    {TYPE_FILL,     ID_SKY,          0,   0,   320, 168,  .data.fill = {COLOR_SKY_LIGHT},        { MOVE_NO }, true},
    {TYPE_FILL,     ID_GROUND,       0, 169,   320,  72,  .data.fill = {COLOR_GROUND_LIGHT},     { MOVE_NO }, true},
};

const uint16_t elements_count = sizeof(elements_list) / sizeof(element_t);

// clang-format on

uint16_t framebuffer[34 * 176]; // 5632

int elements_manager_update_full_screen()
{
    for (int i = elements_count - 1; i >= 0; i--)
    {
        const element_t *element = &elements_list[i];
        elements_manager_update_element(element->id);
    }
    return 0;
}

int elements_manager_update_element(element_id_t id)
{
    const element_t *element = elements_manager_find_element(id);
    if (!element)
    {
        return -1;
    }
    if (!element->visible)
    {
        return 0;
    }

    switch (element->type)
    {
    case TYPE_FILL:
        ILI9341_DrawFilledRectangle(element->x, element->y, element->x + element->width, element->y + element->height, element->data.fill.color);
        break;
    case TYPE_SPRITE:
        fb_generate_background(framebuffer, element->x, element->y, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale);
        fb_draw_bitmap(framebuffer, 0, 0, element->width, element->height, element->data.sprite.sprite, element->data.sprite.scale, element->width * element->data.sprite.scale);
        ILI9341_putBitmap(element->x, element->y, element->width * element->data.sprite.scale, element->height * element->data.sprite.scale, 1, framebuffer, element->width * element->data.sprite.scale * element->height * element->data.sprite.scale);
        break;
    case TYPE_BITMAP:
        fb_generate_background(framebuffer, element->x, element->y, 2, 2);
        uint16_t background_color = framebuffer[0];
        ILI9341_putBitmap_with_bg(element->x, element->y, element->width, element->height, element->data.sprite.scale, background_color, element->data.sprite.sprite, element->width * element->height);
        break;
        break;
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

    int width = element->width;
    int height = element->height;
    uint16_t scale = element->data.sprite.scale;
    int x_offset = 0;
    int y_offset = 0;

//    ILI9341_DrawFilledRectangle(200, 100, 210, 130, 0xFF00);
//    ILI9341_DrawFilledRectangle(124, 100, 134, 130, 0xFF00);
    if (target_x < 0)
    {
        x_offset = abs(target_x);
        // width = width * scale + target_x;
        // printf("x_offset: %d, width: %d\n\r", x_offset, width);
    }
    if (target_y < 0)
    {
        y_offset = abs(target_y);
        // height = height + target_y;
        // printf("y_offset: %d, height: %d\n\r", y_offset, height);
    }

    if (target_x + width * scale >= SCREEN_WIDTH)
    {
        x_offset = target_x + width * scale - SCREEN_WIDTH;
        // width = SCREEN_WIDTH - target_x;
        // printf("x_offset: %d, width: %d\n\r", x_offset, width);
    }

    if (target_y + height * scale >= SCREEN_HEIGHT)
    {
        y_offset = target_y + height * scale - SCREEN_HEIGHT;
        // height = SCREEN_HEIGHT - target_y;
        // printf("y_offset: %d, height: %d\n\r", y_offset, height);
    }

    // check if the new values are overlap the last position

    if (abs(element->x - target_x) < width * scale && abs(element->y - target_y) < height * scale)
    {
        uint16_t fb_width = width * scale + abs(element->x - target_x);
        uint16_t fb_height = height * scale + abs(element->y - target_y);

        uint16_t bitmap_x = 0;
        uint16_t bitmap_y = 0;

        fb_generate_background(framebuffer, element->x, element->y, fb_width, fb_height);
        if ((int32_t)element->x - (int32_t)target_x < 0)
        {
            bitmap_x = fb_width - (width * scale);
        }
        else
        {
            bitmap_x = 0;
        }

        if ((int32_t)element->y - (int32_t)target_y < 0)
        {
            bitmap_y = fb_height - (height * scale);
        }
        else
        {
            bitmap_y = 0;
        }
        fb_draw_bitmap(framebuffer, bitmap_x, bitmap_y, width, height, element->data.sprite.sprite, scale, fb_width);

        uint16_t print_x = target_x;
        uint16_t print_y = target_y;
        if (bitmap_x != 0)
        {
            print_x = target_x - bitmap_x;
        }
//        if (bitmap_y != 0)
//        {
//            print_y = target_y - bitmap_y;
//        }

        ILI9341_putBitmap(print_x, print_y, fb_width, fb_height, 1, framebuffer, fb_width * fb_height);
        // ILI9341_putBitmap(element->x + bitmap_x, element->y, fb_width, fb_height, 1, framebuffer, fb_width * fb_height);
        element->x = target_x;
        element->y = target_y;

        return 0;
    }

    fb_generate_background(framebuffer, element->x, element->y, width * scale, height * scale);
    ILI9341_putBitmap(element->x, element->y, width * scale, height * scale, 1, framebuffer, width * scale * height * scale);

    element->x = target_x;
    element->y = target_y;

    fb_generate_background(framebuffer, element->x, element->y, width * scale, height * scale);
    fb_draw_bitmap(framebuffer, 0, 0, width, height, element->data.sprite.sprite, scale, width * scale);

    ILI9341_putBitmap(element->x, element->y, width * scale, height * scale, 1, framebuffer, width * scale * height * scale);

    return 0;
}

int elements_manager_set_visible(element_id_t id, bool visible)
{
    element_t *element = elements_manager_find_element(id);
    if (!element)
    {
        return -1;
    }
    element->visible = visible;
    elements_manager_update_element(id);

    return 0;
}
