#include <stdio.h>

#include "utils.h"
#include "socket.h"
#include "connection.h"

int main(int argc, char *argv[]) {
    progname = argv[0];

    int const queuesize = 100;
    char const *port = "8080";
    if (argc > 1) {
        port = argv[1];
    }

    socket_t *server = socket_create(port, queuesize);
    if (server == NULL) {
        return 1;
    }
    printf("Bound to port %s\n", port);

    while (1) {
        connection_t *conn = socket_accept(server);
        if (conn == NULL) {
            continue;
        }
        printf("Got connection from %s\n", conn->ipaddr);

        connection_send(conn, "Hello, world!");
        connection_close(conn);
    }

    socket_close(server);
    return 0;
}
