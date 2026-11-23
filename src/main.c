#include <stdio.h>
#include <stdlib.h>

#include <cserver/server.h>

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("cserver: starting display server\n");

    if (cserver_start() < 0) {
        fprintf(stderr, "cserver: failed to start\n");
        return EXIT_FAILURE;
    }

    cserver_run();
    cserver_shutdown();

    printf("cserver: stopped\n");

    return EXIT_SUCCESS;
}
