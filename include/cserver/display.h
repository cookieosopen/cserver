#ifndef CSERVER_DISPLAY_H
#define CSERVER_DISPLAY_H

#include <stdint.h>

enum cserver_display_backend {
    CSERVER_DISPLAY_NONE,
    CSERVER_DISPLAY_DRM,
    CSERVER_DISPLAY_FB
};

extern int cserver_display_fd;
extern enum cserver_display_backend cserver_display_backend;

extern uint32_t cserver_display_width;
extern uint32_t cserver_display_height;
extern uint32_t cserver_display_pitch;
extern uint32_t *cserver_display_pixels;

int cserver_display_init(void);
void cserver_display_close(void);

void cserver_display_clear(uint32_t color);
void cserver_display_put_pixel(
    uint32_t x,
    uint32_t y,
    uint32_t color
);

void cserver_display_fill_rect(
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height,
    uint32_t color
);

#endif
