#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

#include "utils.h"

#include "connection.h"

void connection_close(connection_t *conn) {
    close(conn->fd);
    conn->fd = -1;
    free(conn);
}

void connection_send(connection_t const *const conn, char const *const data) {
    int const datasize = strlen(data);
    int const status = send(conn->fd, data, datasize, 0);
    if (status == -1) {
        fprintf(stderr, "%s: accept: %s\n", progname, strerror(errno));
    }
}
