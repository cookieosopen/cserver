#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#include <cserver/desktop/component_api.h>
#include "icon.h"

#define TASKDASH_CONFIG "/etc/cserver/taskdash.conf"

#define TASKDASH_HORIZONTAL_SIZE 40
#define TASKDASH_VERTICAL_SIZE 64

#define TASKDASH_BACKGROUND 0x20242c
#define TASKDASH_ACCENT     0x4285f4
#define TASKDASH_BUTTON     0x353b46

enum taskdash_location {
    TASKDASH_TOP,
    TASKDASH_BOTTOM,
    TASKDASH_LEFT,
    TASKDASH_RIGHT
};

static enum taskdash_location taskdash_location = TASKDASH_LEFT;

static char *trim(char *str)
{
    while (isspace((unsigned char)*str))
        str++;

    char *end = str + strlen(str);

    while (end > str && isspace((unsigned char)end[-1]))
        end--;

    *end = '\0';

    return str;
}

static void taskdash_load_config(void)
{
    FILE *file = fopen(TASKDASH_CONFIG, "r");

    taskdash_location = TASKDASH_BOTTOM;

    if (!file)
        return;

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        char *setting = trim(line);

        if (*setting == '#' || *setting == '\0')
            continue;

        if (strncmp(setting, "LOCATION=", 9) != 0)
            continue;

        char *value = trim(setting + 9);

        if (strcmp(value, "top") == 0) {
            taskdash_location = TASKDASH_TOP;
        } else if (strcmp(value, "bottom") == 0) {
            taskdash_location = TASKDASH_BOTTOM;
        } else if (strcmp(value, "left") == 0) {
            taskdash_location = TASKDASH_LEFT;
        } else if (strcmp(value, "right") == 0) {
            taskdash_location = TASKDASH_RIGHT;
        }

        break;
    }

    fclose(file);
}

static int taskdash_init(void)
{
    taskdash_load_config();

    return 0;
}

static void taskdash_draw(void)
{
    uint32_t screen_width = csrv_desktop_get_width();
    uint32_t screen_height = csrv_desktop_get_height();

    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t width = screen_width;
    uint32_t height = TASKDASH_HORIZONTAL_SIZE;

    switch (taskdash_location) {
    case TASKDASH_TOP:
        break;

    case TASKDASH_BOTTOM:
        if (screen_height > height)
            y = screen_height - height;
        else
            height = screen_height;
        break;

    case TASKDASH_LEFT:
        width = TASKDASH_VERTICAL_SIZE;
        height = screen_height;
        break;

    case TASKDASH_RIGHT:
        width = TASKDASH_VERTICAL_SIZE;
        height = screen_height;

        if (screen_width > width)
            x = screen_width - width;
        else
            width = screen_width;

        break;
    }

    if (!screen_width || !screen_height)
        return;

    csrv_desktop_fill_rect(
        x, y,
        width, height,
        TASKDASH_BACKGROUND
    );

    /*
     * Launcher button.
     */

    if (taskdash_location == TASKDASH_TOP ||
        taskdash_location == TASKDASH_BOTTOM) {
            taskdash_draw_icon(8, y + 8, 24, 24);
    } else {
        taskdash_draw_icon(x + 8, 8, 48, 48);
    }

    /*
     * Accent line.
     */

    if (taskdash_location == TASKDASH_TOP ||
        taskdash_location == TASKDASH_BOTTOM) {

        csrv_desktop_fill_rect(
            x, y + height - 2,
            width, 2,
            TASKDASH_ACCENT
        );

    } else {

        csrv_desktop_fill_rect(
            x + width - 2, y,
            2, height,
            TASKDASH_ACCENT
        );
    }
}

static void taskdash_shutdown(void)
{
}

static const struct cserver_desktop_component taskdash_component = {
    .name = "TaskDash",
    .description = "CServer desktop taskbar",
    .abi_version = CSERVER_DESKTOP_ABI,

    .init = taskdash_init,
    .update = NULL,
    .draw = taskdash_draw,
    .shutdown = taskdash_shutdown
};

CSERVER_DESKTOP_COMPONENT(taskdash_component)