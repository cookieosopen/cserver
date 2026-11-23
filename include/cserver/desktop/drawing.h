#ifndef CSERVER_DESKTOP_DRAWING_H
#define CSERVER_DESKTOP_DRAWING_H

#include <stdint.h>

void csrv_desktop_clear(uint32_t color);

void csrv_desktop_put_pixel(
    uint32_t x,
    uint32_t y,
    uint32_t color
);

void csrv_desktop_fill_rect(
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint32_t color
);

uint32_t csrv_desktop_get_width(void);
uint32_t csrv_desktop_get_height(void);

#endif
