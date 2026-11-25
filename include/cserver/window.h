#ifndef CSERVER_WINDOW_H
#define CSERVER_WINDOW_H

#include <stdint.h>

struct cserver_window {
    uint32_t id;
    int32_t x;
    int32_t y;
    uint32_t width;
    uint32_t height;
    int visible;
    struct cserver_window *next;
};

int cserver_window_init(void);
void cserver_window_shutdown(void);

struct cserver_window *cserver_window_create(
    int32_t x,
    int32_t y,
    uint32_t width,
    uint32_t height
);

void cserver_window_destroy(uint32_t id);

struct cserver_window *cserver_window_get(uint32_t id);

void cserver_window_move(uint32_t id, int32_t x, int32_t y);
void cserver_window_resize(uint32_t id, uint32_t width, uint32_t height);

void cserver_window_show(uint32_t id);
void cserver_window_hide(uint32_t id);

void cserver_window_focus(uint32_t id);
struct cserver_window *cserver_window_get_focused(void);

struct cserver_window *cserver_window_get_topmost(void);

#endif
