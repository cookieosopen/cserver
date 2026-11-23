#ifndef CSERVER_DESKTOP_LOADER_H
#define CSERVER_DESKTOP_LOADER_H

int cserver_desktop_load(const char *path);
void cserver_desktop_update(void);
void cserver_desktop_draw(void);
void cserver_desktop_unload(void);

#endif