#include <stdint.h>

#include <cserver/desktop/component_api.h>

#define TASKDASH_ICON_BACKGROUND 0x353b46
#define TASKDASH_ICON_ACCENT     0x4285f4
#define TASKDASH_ICON_WHITE      0xffffff

void taskdash_draw_icon(uint32_t x, uint32_t y)
{
    /* Icon background */
    csrv_desktop_fill_rect(
        x, y,
        24, 24,
        TASKDASH_ICON_BACKGROUND
    );

    /* Three horizontal menu lines */
    csrv_desktop_fill_rect(
        x + 5, y + 6,
        14, 2,
        TASKDASH_ICON_WHITE
    );

    csrv_desktop_fill_rect(
        x + 5, y + 11,
        14, 2,
        TASKDASH_ICON_ACCENT
    );

    csrv_desktop_fill_rect(
        x + 5, y + 16,
        14, 2,
        TASKDASH_ICON_WHITE
    );
}