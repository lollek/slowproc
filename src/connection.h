#ifndef CONNECTION_H
#define CONNECTION_H

#define INET6_ADDRSTRLEN 46

typedef struct connection {
    int fd;
    char ipaddr[INET6_ADDRSTRLEN];
} connection_t;

/**
 * Sends null terminated data to the client
 */
void connection_send(connection_t const *const conn, char const *const data);

/**
 * Closes and frees a connection.
*/
void connection_close(connection_t *conn);


#endif // CONNECTION_H
