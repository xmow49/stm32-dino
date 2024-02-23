#include "dino/framebuffer.h"
#include <lib/tft_ili9341/stm32f1_ili9341.h>

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

void fb_draw_bitmap(uint16_t *framebuffer, int16_t x0, int16_t y0, int16_t width, int16_t height, const int16_t *img, int32_t size)
{
    for (int16_t y = 0; y < height; y++)
    {

        for (int16_t x = 0; x < width; x++)
        {
            if (img[y * width + x] == 0)
            {
                continue;
            }

            framebuffer[y * width + x] = img[y * width + x];
        }
    }
}
