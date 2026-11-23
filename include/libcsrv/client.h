#ifndef LIBCSRV_CLIENT_H
#define LIBCSRV_CLIENT_H

typedef struct csrv_connection {
    int fd;
} csrv_connection;

int csrv_connect(csrv_connection *connection);
void csrv_disconnect(csrv_connection *connection);

#endif
