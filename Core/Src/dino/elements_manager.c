
#include <stdint.h>

#include "dino/elements_manager.h"
#include "dino/sprites.h"
#include "dino/dino.h"
#include "dino/framebuffer.h"
#include "dino/move_manager.h"
#include "dino/score.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <lib/tft_ili9341/stm32f1_ili9341.h>

// clang-format off
element_t elements_list[] = {
    // TYPE,        ID,             X,              Y,              WIDTH,  HEIGHT,   DATA,                                     MOVE,   VISIBLE
    {TYPE_SPRITE,   ID_CROWN,       220,            8,              18,     15,     .data.sprite = {1, sprite_crown},       { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_DINO_STAND,  D_X,            D_Y,            20,     22,     .data.sprite = {2, sprite_dino_stand},  { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_DINO_SIT,    D_X,            D_Y+20,         28,     12,     .data.sprite = {2, sprite_dino_sit},    { MOVE_NO }, false},

    {TYPE_SPRITE,   ID_BIRD,        B_X_START,      B_Y_START,      21,     16,     .data.sprite = {2, sprite_bird_down},   { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_CACTUS,      C_X_START,      C_Y_START,      9,      19,     .data.sprite = {2, sprite_cactus},      { MOVE_NO }, true},

    {TYPE_SPRITE,   ID_GAME_OVER,   48,             53,             73,     9,      .data.sprite = {3, sprite_gameover},    { MOVE_NO }, false}, //5913
    {TYPE_SPRITE,   ID_PRESS_RETRY, 123,            95,             73,     19,     .data.sprite = {1, sprite_press_start}, { MOVE_NO }, false}, 
    {TYPE_SPRITE,   ID_PRESS_START, 87,             50,             73,     19,     .data.sprite = {2, sprite_press_start}, { MOVE_NO }, true}, 

    {TYPE_BITMAP,   ID_COPYRIGHT,   31,             178,            129,    26,     .data.sprite = {2, sprite_copyright},   { MOVE_NO }, true},
    
    {TYPE_SPRITE,   ID_ROCK_0,      45,             165,            11,     3,      .data.sprite = {1, sprite_rock},        { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_ROCK_1,      212,            165,            11,     3,      .data.sprite = {1, sprite_rock},        { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_ROCK_2,      308,            165,            11,     3,      .data.sprite = {1, sprite_rock},        { MOVE_NO }, true},

    {TYPE_SPRITE,   ID_HOLE_0,      138,            168,            12,     2,      .data.sprite = {1, sprite_hole},        { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_HOLE_1,      252,            168,            12,     2,      .data.sprite = {1, sprite_hole},        { MOVE_NO }, true},
    
    {TYPE_SPRITE,   ID_MOON,        138,            38,             19,     19,     .data.sprite = {2, sprite_moon},        { MOVE_NO }, false},
    {TYPE_SPRITE,   ID_CLOUD_1,     CLOUD_X_START,  CLOUD_Y_START,  28,     13,     .data.sprite = {2, sprite_cloud},       { MOVE_NO }, true},
    {TYPE_SPRITE,   ID_CLOUD_0,     CLOUD_X_START,  CLOUD_Y_START,  28,     13,     .data.sprite = {2, sprite_cloud},       { MOVE_NO }, true},
    {TYPE_FILL,     ID_SKY,         0,              0,              320,    168,    .data.fill = {COLOR_SKY_LIGHT},         { MOVE_NO }, true},
    {TYPE_FILL,     ID_GROUND,      0,              169,            320,    72,     .data.fill = {COLOR_GROUND_LIGHT},      { MOVE_NO }, true},
};

const uint16_t elements_count = sizeof(elements_list) / sizeof(element_t);

static bool dark_mode = false;

// clang-format on

uint16_t framebuffer[34 * 176]; // 5632

int elements_manager_update_full_screen()
{
    for (int i = elements_count - 1; i >= 0; i--)
    {
        const element_t *element = &elements_list[i];
        elements_manager_update_element(element->id);
    }
    score_draw();
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

    int16_t fb_width = element->width * element->data.sprite.scale;
    int16_t fb_height = element->height * element->data.sprite.scale;

    switch (element->type)
    {
    case TYPE_FILL:
        ILI9341_DrawFilledRectangle(element->x, element->y, element->x + element->width, element->y + element->height, element->data.fill.color);
        break;

    case TYPE_SPRITE:

        fb_generate_background(framebuffer, element->x, element->y, fb_width, fb_height);
        fb_draw_bitmap(framebuffer, 0, 0, element->width, element->height, element->data.sprite.sprite, element->data.sprite.scale, fb_width);
        ILI9341_putBitmap(element->x, element->y, fb_width, fb_height, 1, framebuffer, fb_width * fb_height);
        break;
    case TYPE_BITMAP:
        fb_generate_background(framebuffer, element->x, element->y, 2, 2);
        uint16_t background_color = framebuffer[0];
        ILI9341_putBitmap_with_bg(element->x, element->y, element->width, element->height, element->data.sprite.scale, background_color, element->data.sprite.sprite, element->width * element->height);
        break;
        break;
    default:
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

        uint16_t print_x = target_x;
        uint16_t print_y = target_y;
        if (bitmap_x != 0)
        {
            print_x = target_x - bitmap_x;
        }
        if (bitmap_y != 0)
        {
            print_y = target_y - bitmap_y;
        }

        if (element->visible)
        {
            fb_generate_background(framebuffer, print_x, print_y, fb_width, fb_height);
            fb_draw_bitmap(framebuffer, bitmap_x, bitmap_y, width, height, element->data.sprite.sprite, scale, fb_width);
            ILI9341_putBitmap(print_x, print_y, fb_width, fb_height, 1, framebuffer, fb_width * fb_height);
        }
        // ILI9341_DrawRectangle(print_x - 1, print_y - 1, print_x + fb_width + 1, print_y + fb_height + 1, 0x0000);
        // ILI9341_putBitmap(element->x + bitmap_x, element->y, fb_width, fb_height, 1, framebuffer, fb_width * fb_height);
        element->x = target_x;
        element->y = target_y;

        return 0;
    }

    if (element->visible)
    {
        fb_generate_background(framebuffer, element->x, element->y, width * scale, height * scale);
        ILI9341_putBitmap(element->x, element->y, width * scale, height * scale, 1, framebuffer, width * scale * height * scale);
    }
    element->x = target_x;
    element->y = target_y;

    if (element->visible)
    {
        fb_generate_background(framebuffer, element->x, element->y, width * scale, height * scale);
        fb_draw_bitmap(framebuffer, 0, 0, width, height, element->data.sprite.sprite, scale, width * scale);
        ILI9341_putBitmap(element->x, element->y, width * scale, height * scale, 1, framebuffer, width * scale * height * scale);
    }

    return 0;
}

int elements_manager_clear_element(element_id_t id)
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

    fb_generate_background(framebuffer, element->x, element->y, width * scale, height * scale);
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

    if (visible)
    {
        elements_manager_update_element(id);
    }
    else
    {
        elements_manager_clear_element(id);
    }

    return 0;
}

int elements_manager_set_dark_mode(bool state)
{
    if (dark_mode == state)
    {
        return 0;
    }

    element_t *sky = elements_manager_find_element(ID_SKY);
    element_t *ground = elements_manager_find_element(ID_GROUND);
    element_t *moon = elements_manager_find_element(ID_MOON);
    dark_mode = state;

    if (state)
    {
        sky->data.fill.color = COLOR_SKY_DARK;
        ground->data.fill.color = COLOR_GROUND_DARK;
        elements_manager_set_visible(ID_MOON, true);
        elements_manager_set_visible(ID_CLOUD_0, false);
        elements_manager_set_visible(ID_CLOUD_1, false);
        elements_manager_update_full_screen();
    }
    else
    {
        sky->data.fill.color = COLOR_SKY_LIGHT;
        ground->data.fill.color = COLOR_GROUND_LIGHT;
        elements_manager_set_visible(ID_MOON, false);
        elements_manager_set_visible(ID_CLOUD_0, true);
        elements_manager_set_visible(ID_CLOUD_1, true);
        elements_manager_update_full_screen();
    }
    return 0;
}

bool elements_manager_get_dark_mode()
{
    return dark_mode;
}
