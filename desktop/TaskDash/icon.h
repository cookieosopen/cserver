#ifndef TASKDASH_ICON_H
#define TASKDASH_ICON_H

#include <stdint.h>

int taskdash_icon_load(const char *path);

void taskdash_draw_icon(
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height
);

void taskdash_icon_unload(void);

#endif