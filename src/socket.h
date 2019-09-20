#ifndef SOCKET_H
#define SOCKET_H

#define INET6_ADDRSTRLEN 46

typedef struct socket {
    int fd;
    char ipaddr[INET6_ADDRSTRLEN];
} socket_t;

/**
 * Creates and binds a socket. Note that you must call socket_close() on the
 * created socket when finished
 *
 * @hostname    Hostname to connect to. NULL to bind to localhost.
 * @port        Port to bind to.
 * @queuesize   Number of items in queue backlog before we start dropping
 *              packets.
 * @RETURN      A socket on success, else NULL.
 */
socket_t *socket_create(char const *restrict hostname, char const *restrict port,
        int queuesize);

/**
 * Accepts a new connection on a socket. Note that you must call
 * socket_close() on the created socket when finished.
 *
 * @socket      Socket on which to accept connections
 * @RETURN      A socket on success, else NULL.
 */
socket_t *socket_accept(socket_t const *const socket);

/**
 * Sends a string of null terminated data on a socket.
 */
void socket_send(socket_t const *const socket, char const *const data);

/**
 * Joins two sockets with each other, so that data from one gets sent to the
 * other. Returns when one of the sockets closes connection.
 */
void socket_join(socket_t *first, socket_t *second);

/**
 * Closes and frees a socket.
*/
void socket_close(socket_t *socket);

#endif // SOCKET_H
