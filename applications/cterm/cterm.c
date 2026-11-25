#include <stdio.h>
#include <stdlib.h>

#include <libcsrv/csrv.h>

int main(void)
{
    csrv_connection connection = {
        .fd = -1
    };

    if (csrv_connect(&connection) < 0) {
        fprintf(stderr, "cterm: failed to connect to CServer\n");
        return EXIT_FAILURE;
    }

    printf("cterm: connected to CServer\n");

    csrv_disconnect(&connection);

    return EXIT_SUCCESS;
}
