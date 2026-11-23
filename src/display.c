#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <cserver/display.h>

int cserver_display_fd = -1;

enum cserver_display_backend cserver_display_backend = CSERVER_DISPLAY_NONE;

int cserver_display_init(void)
{
    printf("cserver: initializing display...\n");

    printf("cserver: trying DRM/KMS (/dev/dri/card0)\n");

    cserver_display_fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);

    if (cserver_display_fd >= 0) {
        /*
         * DRM device opened.
         * DRM/KMS initialization will go here.
         */
        cserver_display_backend = CSERVER_DISPLAY_DRM;

        printf("cserver: DRM device opened\n");
        return 0;
    }

    printf("cserver: DRM unavailable: %s\n", strerror(errno));

    printf("cserver: trying framebuffer (/dev/fb0)\n");

    cserver_display_fd = open("/dev/fb0", O_RDWR | O_CLOEXEC);

    if (cserver_display_fd >= 0) {
        cserver_display_backend = CSERVER_DISPLAY_FB;

        printf("cserver: framebuffer opened\n");
        return 0;
    }

    printf("cserver: framebuffer unavailable: %s\n", strerror(errno));
    printf("cserver: no usable display backend found\n");

    return -1;
}

void cserver_display_close(void)
{
    if (cserver_display_fd >= 0) {
        close(cserver_display_fd);
        cserver_display_fd = -1;
    }

    cserver_display_backend = CSERVER_DISPLAY_NONE;
}
