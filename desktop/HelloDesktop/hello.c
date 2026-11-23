#include <cserver/desktop/component_api.h>

static int hello_init(void)
{
    return 0;
}

static void hello_update(void)
{
}

static void hello_draw(void)
{
    csrv_desktop_fill_rect(
        20, 20, 300, 80,
        0x202020
    );

    csrv_desktop_fill_rect(
        30, 30, 280, 60,
        0x3366ff
    );
}

static void hello_shutdown(void)
{
}

static const struct cserver_desktop_component hello_component = {
    .name = "HelloDesktop",
    .description = "Example CServer desktop component",
    .abi_version = CSERVER_DESKTOP_ABI,

    .init = hello_init,
    .update = hello_update,
    .draw = hello_draw,
    .shutdown = hello_shutdown
};

CSERVER_DESKTOP_COMPONENT(hello_component)