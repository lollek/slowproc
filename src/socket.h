#ifndef SOCKET_H
#define SOCKET_H

#include "connection.h"

typedef struct socket {
    int fd;
} socket_t;

/**
 * Creates and binds a socket. Note that you must call socket_close() when
 * finished
 *
 * @port        Port to bind to.
 * @queuesize   Number of items in queue backlog before we start dropping
 *              packets.
 * @RETURN      A socket on success, else NULL.
 */
socket_t *socket_create(char const *restrict port, int queuesize);

/**
 * Accepts a new connection on a socket. Note that you must call
 * connection_close() when finished.
 *
 * @socket      Socket on which to accept connections
 * @RETURN      A connection on success, else NULL.
 */
connection_t *socket_accept(socket_t const *const socket);

/**
 * Closes and frees a socket.
*/
void socket_close(socket_t *socket);

#endif // SOCKET_H
