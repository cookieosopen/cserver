#include <libcsrv/client.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

#define CSRV_SOCKET_PATH "/run/cserver.sock"

int csrv_connect(csrv_connection *connection)
{
    if (!connection)
        return -1;

    struct sockaddr_un address = {
        .sun_family = AF_UNIX
    };

    connection->fd = -1;

    strncpy(address.sun_path, CSRV_SOCKET_PATH,
            sizeof(address.sun_path) - 1);

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd < 0)
        return -1;

    if (connect(fd, (struct sockaddr *)&address,
                sizeof(address)) < 0) {
        close(fd);
        return -1;
    }

    connection->fd = fd;

    return 0;
}

void csrv_disconnect(csrv_connection *connection)
{
    if (!connection || connection->fd < 0)
        return;

    close(connection->fd);
    connection->fd = -1;
}