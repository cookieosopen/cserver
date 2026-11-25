#include <cserver/window.h>

#include <stdlib.h>
#include <stddef.h>

static struct cserver_window *window_list;
static struct cserver_window *focused_window;

static uint32_t next_window_id = 1;

int cserver_window_init(void)
{
    window_list = NULL;
    focused_window = NULL;
    next_window_id = 1;

    return 0;
}

void cserver_window_shutdown(void)
{
    struct cserver_window *window = window_list;

    while (window) {
        struct cserver_window *next = window->next;

        free(window);
        window = next;
    }

    window_list = NULL;
    focused_window = NULL;
}

struct cserver_window *cserver_window_get(uint32_t id)
{
    struct cserver_window *window = window_list;

    while (window) {
        if (window->id == id)
            return window;

        window = window->next;
    }

    return NULL;
}

struct cserver_window *cserver_window_create(
    int32_t x,
    int32_t y,
    uint32_t width,
    uint32_t height
)
{
    if (!width || !height)
        return NULL;

    struct cserver_window *window = calloc(1, sizeof(*window));

    if (!window)
        return NULL;

    window->id = next_window_id++;

    if (!window->id)
        window->id = next_window_id++;

    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->visible = 1;

    window->next = window_list;
    window_list = window;

    cserver_window_focus(window->id);

    return window;
}

void cserver_window_destroy(uint32_t id)
{
    struct cserver_window **link = &window_list;

    while (*link) {
        struct cserver_window *window = *link;

        if (window->id == id) {
            *link = window->next;

            if (focused_window == window)
                focused_window = window_list;

            free(window);
            return;
        }

        link = &window->next;
    }
}

void cserver_window_move(uint32_t id, int32_t x, int32_t y)
{
    struct cserver_window *window = cserver_window_get(id);

    if (!window)
        return;

    window->x = x;
    window->y = y;
}

void cserver_window_resize(uint32_t id, uint32_t width, uint32_t height)
{
    struct cserver_window *window = cserver_window_get(id);

    if (!window || !width || !height)
        return;

    window->width = width;
    window->height = height;
}

void cserver_window_show(uint32_t id)
{
    struct cserver_window *window = cserver_window_get(id);

    if (window)
        window->visible = 1;
}

void cserver_window_hide(uint32_t id)
{
    struct cserver_window *window = cserver_window_get(id);

    if (!window)
        return;

    window->visible = 0;

    if (focused_window == window)
        focused_window = NULL;
}

void cserver_window_focus(uint32_t id)
{
    struct cserver_window *window = cserver_window_get(id);

    if (!window || !window->visible)
        return;

    if (window_list == window) {
        focused_window = window;
        return;
    }

    struct cserver_window **link = &window_list;

    while (*link && *link != window)
        link = &(*link)->next;

    if (!*link)
        return;

    *link = window->next;

    window->next = window_list;
    window_list = window;

    focused_window = window;
}

struct cserver_window *cserver_window_get_focused(void)
{
    return focused_window;
}

struct cserver_window *cserver_window_get_topmost(void)
{
    struct cserver_window *window = window_list;

    while (window) {
        if (window->visible)
            return window;

        window = window->next;
    }

    return NULL;
}
