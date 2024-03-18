#include "dino/framebuffer.h"
#include <lib/tft_ili9341/stm32f1_ili9341.h>
#include "dino/elements_manager.h"

void fb_send_full_screen()
{
    // ILI9341_putBitmap(0, 0, 320, 240, 1, framebuffer, 320 * 240);
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