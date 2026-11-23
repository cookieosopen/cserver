#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <cserver/server.h>
#include <cserver/display.h>
#include <cserver/desktop/loader.h>

static volatile sig_atomic_t cserver_running = 0;

int cserver_start(void)
{
    printf("cserver: initializing server\n");

    if (cserver_display_init() < 0) {
        fprintf(stderr, "cserver: display initialization failed\n");
        return -1;
    }

    printf("cserver: loading desktop components [1/2] \n");
    cserver_desktop_load("/usr/lib/cserver/desktop");

    printf("cserver: drawing desktop [2/2] \n");
    cserver_desktop_draw();

    cserver_running = 1;

    printf("cserver: server initialized\n");

    return 0;
}

void cserver_run(void)
{
    printf("cserver: entering event loop\n");

    while (cserver_running) {
        pause();
    }
}

void cserver_shutdown(void)
{
    printf("cserver: shutting down\n");

    cserver_running = 0;

    printf("cserver: desktop components unloading\n");
    cserver_desktop_unload();

    cserver_display_close();
}
