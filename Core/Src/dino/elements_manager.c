
#include <stdint.h>

#include "dino/elements_manager.h"
#include "dino/sprites.h"
#include "dino/dino.h"
#include "dino/framebuffer.h"

typedef enum
{
    TYPE_FILL,
    TYPE_SPRITE,
    TYPE_TEXT,
} type_t;

typedef enum
{
    ID_SKY,
    ID_GROUND,

    ID_CACTUS_0,
    ID_CACTUS_1,
    ID_CACTUS_2,

    ID_CLOUD_0,
    ID_CLOUD_1,

    ID_DINO,
    ID_BIRD,

    ID_COPYRIGHT,
    ID_GAME_OVER,

} element_id_t;

typedef struct
{
    type_t type;
    element_id_t id;
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    union
    {
        struct
        {
            uint16_t color;
        } fill;
        struct
        {
            uint16_t scale;
            const uint16_t *sprite;
        } sprite;
        struct
        {
            uint16_t scale;
            const char *text;
        } text;
    } data;

} element_t;

// clang-format off
const element_t elements_list[] = {
    // {TYPE_SPRITE,   ID_DINO,        82,  124,    20, 22,  .data.sprite = {2, sprite_dino_stand}},
    // {TYPE_SPRITE,   ID_CACTUS_0,   151,  132,    9,  19,  .data.sprite = {2, sprite_cactus}},

    {TYPE_SPRITE,   ID_CACTUS_1,   200,  140,    9,  19,  .data.sprite = {2, sprite_cactus}},


    // {TYPE_SPRITE,   ID_GAME_OVER,   48,  53,    73,  9,  .data.sprite = {3, sprite_gameover}},

    // {TYPE_SPRITE,   ID_COPYRIGHT,   31,  175,   129,  26,  .data.sprite = {2, sprite_copyright}},


    {TYPE_FILL,     ID_SKY,         0,  0,     320, 168,   .data.fill = {COLOR_SKY_LIGHT}},
    {TYPE_FILL,     ID_GROUND,      0,  168,   320, 72,    .data.fill = {COLOR_GROUND_LIGHT}},
};

const uint16_t elements_count = sizeof(elements_list) / sizeof(element_t);

// clang-format on

uint16_t framebuffer[130 * 30];

int elements_manager_update_full_screen()
{
    for (int i = elements_count - 1; i >= 0; i--)
    {
        const element_t *element = &elements_list[i];
        if (element->type == TYPE_FILL)
        {
            ILI9341_DrawFilledRectangle(element->x, element->y, element->x + element->width, element->y + element->height, element->data.fill.color);
        }
        else if (element->type == TYPE_SPRITE)
        {
            // add backgound to the framebuffer
            uint16_t framebuffer_x_offset = element->x;
            uint16_t framebuffer_y_offset = element->y;
            uint16_t framebuffer_index = 0;
            for (int i = elements_count - 1; i >= 0; i--)
            {
                const element_t *bg_element = &elements_list[i];
                if (bg_element->type != TYPE_FILL)
                {
                    continue;
                }
                for (int y = framebuffer_y_offset; y < framebuffer_y_offset + element->height; y++)
                {
                    for (int x = framebuffer_x_offset; x < framebuffer_x_offset + element->width; x++)
                    {
                        // framebuffer[framebuffer_index] = bg_element->data.fill.color;
                        if (x > bg_element->x && x < bg_element->x + bg_element->width && y > bg_element->y && y < bg_element->y + bg_element->height)
                        {
                            framebuffer[framebuffer_index] = bg_element->data.fill.color;
                            framebuffer_index++;
                        }
                    }
                }
            }

            fb_draw_bitmap(framebuffer, element->x, element->y, element->width, element->height, element->data.sprite.sprite, element->width * element->height);
            ILI9341_putBitmap(element->x, element->y, element->width, element->height, element->data.sprite.scale, framebuffer, element->width * element->height);
        }
    }
    return 0;
}