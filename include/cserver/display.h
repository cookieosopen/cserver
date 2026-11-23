
#ifndef CSERVER_DISPLAY_H
#define CSERVER_DISPLAY_H

enum cserver_display_backend {
    CSERVER_DISPLAY_NONE,
    CSERVER_DISPLAY_DRM,
    CSERVER_DISPLAY_FB
};

extern int cserver_display_fd;
extern enum cserver_display_backend cserver_display_backend;

int cserver_display_init(void);
void cserver_display_close(void);

#endif
