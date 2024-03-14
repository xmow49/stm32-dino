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

void fb_draw_bitmap(uint16_t *framebuffer, int16_t x0, int16_t y0, int16_t width, int16_t height, const uint16_t *img, uint8_t scale)
{
    uint32_t fb_index = 0;
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
                    	fb_index++;
                        continue;
                    }
                    framebuffer[fb_index++] = img[y * width + x];
                }
            }
        }
    }

    fb_index++;
}
