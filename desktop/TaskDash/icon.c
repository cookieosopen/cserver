#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdint.h>
#include <stdlib.h>

#include <cserver/desktop/component_api.h>

#include "icon.h"

static uint8_t *taskdash_icon;
static int taskdash_icon_width;
static int taskdash_icon_height;

int taskdash_icon_load(const char *path)
{
    if (!path)
        return -1;

    int width;
    int height;
    int channels;

    uint8_t *image = stbi_load(
        path,
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!image)
        return -1;

    taskdash_icon_unload();

    taskdash_icon = image;
    taskdash_icon_width = width;
    taskdash_icon_height = height;

    return 0;
}

void taskdash_draw_icon(
    uint32_t x,
    uint32_t y,
    uint32_t width,
    uint32_t height
)
{
    if (!taskdash_icon || !width || !height)
        return;

    for (uint32_t dy = 0; dy < height; dy++) {
        for (uint32_t dx = 0; dx < width; dx++) {
            uint32_t sx = dx * taskdash_icon_width / width;
            uint32_t sy = dy * taskdash_icon_height / height;

            uint8_t *pixel = taskdash_icon +
                ((size_t)sy * taskdash_icon_width + sx) * 4;

            uint8_t r = pixel[0];
            uint8_t g = pixel[1];
            uint8_t b = pixel[2];
            uint8_t a = pixel[3];

            if (!a)
                continue;

            if (a < 255) {
                const uint32_t background = 0x20242c;

                uint32_t br = (background >> 16) & 0xff;
                uint32_t bg = (background >> 8) & 0xff;
                uint32_t bb = background & 0xff;

                r = (r * a + br * (255 - a)) / 255;
                g = (g * a + bg * (255 - a)) / 255;
                b = (b * a + bb * (255 - a)) / 255;
            }

            uint32_t color =
                ((uint32_t)r << 16) |
                ((uint32_t)g << 8) |
                b;

            csrv_desktop_put_pixel(x + dx, y + dy, color);
        }
    }
}

void taskdash_icon_unload(void)
{
    if (taskdash_icon) {
        stbi_image_free(taskdash_icon);
        taskdash_icon = NULL;
    }

    taskdash_icon_width = 0;
    taskdash_icon_height = 0;
}