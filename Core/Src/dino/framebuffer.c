#include "dino/framebuffer.h"
#include <lib/tft_ili9341/stm32f1_ili9341.h>
#include "dino/elements_manager.h"

void fb_send_full_screen()
{
    // ILI9341_putBitmap(0, 0, 320, 240, 1, framebuffer, 320 * 240);
}

void fb_generate_background(uint16_t *framebuffer, uint16_t framebuffer_x_offset, uint16_t framebuffer_y_offset, uint16_t width, uint16_t height)
{
    uint16_t bg_color = 0x0000;
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
            bg_color = bg_element->data.fill.color;
            for (int x = framebuffer_x_offset; x < framebuffer_x_offset + (width); x++)
            {
                if ((x >= bg_element->x && x <= bg_max_x && y >= bg_element->y && y <= bg_max_y))
                {
                    framebuffer[(y - framebuffer_y_offset) * width + (x - framebuffer_x_offset)] = bg_element->data.fill.color;
                }
                if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT)
                {
                    framebuffer[(y - framebuffer_y_offset) * width + (x - framebuffer_x_offset)] = bg_color;
                }
            }
        }
    }
}

void fb_draw_filled_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    // for (uint16_t y = y0; y < y1; y++)
    // {
    //     for (uint16_t x = x0; x < x1; x++)
    //     {
    //         framebuffer[y * 320 + x] = color;
    //     }
    // }
}

void fb_draw_bitmap(uint16_t *framebuffer, int16_t x0, int16_t y0, int16_t width, int16_t height, const uint16_t *img, uint8_t scale, uint16_t fb_width)
{
    for (int32_t y = 0; y < height; y++)
    {
        for (int32_t k = 0; k < scale; k++)
        {
            for (int32_t x = 0; x < width; x++)
            {
                for (int32_t j = 0; j < scale; j++)
                {
                    if (img[y * width + x] == 0)
                    {
                        continue;
                    }

                    framebuffer[(y0 + y * scale + k) * fb_width + x0 + x * scale + j] = img[y * width + x];
                }
            }
        }
    }
}

// void fb_crop(uint16_t *framebuffer, int16_t *fb_width, int16_t *fb_height, int16_t x_offset, int16_t y_offset)
//{
//     // crop all the framebuffer if we exceed the screen size
//
//     if (x_offset + *fb_width > SCREEN_WIDTH)
//     {
//         int16_t to_crop = x_offset + *fb_width - SCREEN_WIDTH;
////        fb_cut_bitmap(framebuffer, fb_width, fb_height, to_crop, 0);
//    }
//}

// void fb_cut_bitmap(uint16_t *framebuffer, int16_t *fb_width, int16_t *fb_height, int16_t x_cut, int16_t y_cut)
//{
//     // reduce the framebuffer size by cutting x_cut pixels on the right and y_cut pixels on the bottom
//
//     int16_t new_fb_width = *fb_width - x_cut;
//     int16_t new_fb_height = *fb_height - y_cut;
//
//     if (new_fb_width < 0)
//     {
//         return;
//     }
//     if (new_fb_height < 0)
//     {
//         return;
//     }
//
//     for (int32_t y = 0; y < new_fb_height; y++)
//     {
//         for (int32_t x = 0; x < new_fb_width; x++)
//         {
//             uint32_t index = y * new_fb_width + x;
//             uint32_t old_index = y * (*fb_width) + x;
//             framebuffer[index] = framebuffer[old_index];
//         }
//     }
//
//     *fb_width = new_fb_width;
//     *fb_height = new_fb_height;
// }
void fb_shrink_bitmap(uint16_t *framebuffer, int16_t *fb_width, int16_t *fb_height, int16_t x_offset, int16_t y_offset)
{
    // Vérifie si le décalage est négatif
    if (x_offset < 0)
    {
        // Calcule la nouvelle largeur du framebuffer
        int16_t new_fb_width = *fb_width + x_offset;

        // Si la nouvelle largeur est négative, ajuste-la à 0
        if (new_fb_width < 0)
        {
            new_fb_width = 0;
        }

        // Décale les pixels du framebuffer vers la gauche
        for (int32_t y = 0; y < *fb_height; y++)
        {
            for (int32_t x = 0; x < new_fb_width; x++)
            {
                framebuffer[y * (*fb_width) + x] = framebuffer[y * (*fb_width) + x - x_offset];
            }
        }
        // Met à jour la largeur du framebuffer
        *fb_width = new_fb_width;
    }
}
