#ifndef LIBCSRV_PROTOCOL_H
#define LIBCSRV_PROTOCOL_H

#include <stdint.h>

#define CSRV_SOCKET_PATH "/run/cserver.sock"

enum csrv_message_type {
    CSRV_CREATE_WINDOW = 1,
    CSRV_DESTROY_WINDOW,
    CSRV_MOVE_WINDOW,
    CSRV_RESIZE_WINDOW,
    CSRV_UPDATE_WINDOW,
    CSRV_GET_DISPLAY
};

struct csrv_message {
    uint32_t type;
    uint32_t size;
};

#endif
