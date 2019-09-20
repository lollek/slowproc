#include <stdio.h>

#include "utils.h"
#include "socket.h"

int main(int argc, char *argv[]) {
    progname = argv[0];

    int const queuesize = 100;
    char const *listen_port = "8080";
    if (argc > 1) {
        listen_port = argv[1];
    }

    char const *target_hostname = "google.com";
    if (argc > 2) {
        target_hostname = argv[2];
    }

    char const *target_port = "80";
    if (argc > 3) {
        target_port = argv[3];
    }

    socket_t *listen_server = socket_create(NULL, listen_port, queuesize);
    if (listen_server == NULL) {
        return 1;
    }
    printf("Listening on port %s\n", listen_port);

    while (1) {
        socket_t *conn = socket_accept(listen_server);
        if (conn == NULL) {
            continue;
        }
        printf("Got connection from %s\n", conn->ipaddr);

        socket_t *remote_server = socket_create(target_hostname, target_port, queuesize);
        if (remote_server == NULL) {
            return 1;
        }
        printf("Connected to port %s:%s\n", target_hostname, target_port);

        socket_join(conn, remote_server);

        socket_close(conn);
        socket_close(remote_server);

        printf("Closed connections\n");
    }

    socket_close(listen_server);
    return 0;
}
