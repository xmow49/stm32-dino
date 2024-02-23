

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>

void fb_draw_filled_rectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

void fb_draw_bitmap(uint16_t *framebuffer, int16_t x0, int16_t y0, int16_t width, int16_t height, const int16_t *img, int32_t size);

#endif // FRAMEBUFFER_H