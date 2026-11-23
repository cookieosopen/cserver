#include <stdint.h>
#include <stddef.h>

#include <cserver/desktop/component_api.h>
#include "cursor.h"

#define CURSOR_WIDTH 16
#define CURSOR_HEIGHT 16

static int32_t mouse_x = 100;
static int32_t mouse_y = 100;

static int mouse_visible = 1;

static const char *const cursor_shape[CURSOR_HEIGHT] = {
    "X...............",
    "XX..............",
    "XWX.............",
    "XWWX............",
    "XWWWX...........",
    "XWWWWX..........",
    "XWWWWWX.........",
    "XWWWWWWX........",
    "XWWWWWWWX.......",
    "XWWWWXXXX.......",
    "XWWX.X..........",
    "XWX..XX.........",
    "XX....X.........",
    "X.....XX........",
    "......X.........",
    "......X........."
};

void desktop_mouse_move(int32_t x, int32_t y)
{
    if (x < 0)
        x = 0;

    if (y < 0)
        y = 0;

    uint32_t width = csrv_desktop_get_width();
    uint32_t height = csrv_desktop_get_height();

    if (width && (uint32_t)x >= width)
        x = width - 1;

    if (height && (uint32_t)y >= height)
        y = height - 1;

    mouse_x = x;
    mouse_y = y;
}

void desktop_mouse_show(void)
{
    mouse_visible = 1;
}

void desktop_mouse_hide(void)
{
    mouse_visible = 0;
}

static int mouse_init(void)
{
    mouse_x = 100;
    mouse_y = 100;
    mouse_visible = 1;

    return 0;
}

static void mouse_draw(void)
{
    if (!mouse_visible)
        return;

    for (uint32_t y = 0; y < CURSOR_HEIGHT; y++) {
        for (uint32_t x = 0; x < CURSOR_WIDTH; x++) {
            char pixel = cursor_shape[y][x];

            if (pixel == 'X') {
                csrv_desktop_put_pixel(
                    mouse_x + x,
                    mouse_y + y,
                    0x000000
                );
            } else if (pixel == 'W') {
                csrv_desktop_put_pixel(
                    mouse_x + x,
                    mouse_y + y,
                    0xffffff
                );
            }
        }
    }
}

static void mouse_shutdown(void)
{
    mouse_visible = 0;
}

static const struct cserver_desktop_component mouse_component = {
    .name = "DesktopMouse",
    .description = "CServer desktop mouse cursor",
    .abi_version = CSERVER_DESKTOP_ABI,

    .init = mouse_init,
    .update = NULL,
    .draw = mouse_draw,
    .shutdown = mouse_shutdown
};

CSERVER_DESKTOP_COMPONENT(mouse_component)
