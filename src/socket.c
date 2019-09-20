#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#include "utils.h"

#include "socket.h"

/**
 * Returns IPV4 or IPV6 sockaddr
 */
static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void socket_close(socket_t *socket) {
    close(socket->fd);
    socket->fd = -1;
    free(socket);
}

connection_t *socket_accept(socket_t const *const socket) {
    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof(their_addr);
    int const fd = accept(socket->fd, (struct sockaddr *)&their_addr, &sin_size);
    if (fd == -1) {
        fprintf(stderr, "%s: accept: %s\n", progname, strerror(errno));
        return NULL;
    }

    char ipaddr_str[INET6_ADDRSTRLEN];
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),
            ipaddr_str, sizeof ipaddr_str);

    connection_t *conn = xmalloc(sizeof(connection_t));
    conn->fd = fd;
    strcpy(conn->ipaddr, ipaddr_str);
    return conn;
}


socket_t *socket_create(char const *restrict port, int queuesize) {
    struct addrinfo hints = {
        .ai_flags = AI_PASSIVE,
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = 0,
        .ai_addrlen = 0,
        .ai_addr = NULL,
        .ai_canonname = NULL,
        .ai_next = NULL,
    };

    struct addrinfo *servinfo;
    int const status = getaddrinfo(NULL, port, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return NULL;
    }

    int sockfd;
    struct addrinfo *iter;
    for (iter = servinfo; iter != NULL; iter = iter->ai_next) {
        sockfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
        if (sockfd == -1) {
            fprintf(stderr, "%s: socket: %s\n", progname, strerror(errno));
            continue;
        }

        int const yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            fprintf(stderr, "%s: setsockopt: %s\n", progname, strerror(errno));
            return NULL;
        }

        if (bind(sockfd, iter->ai_addr, iter->ai_addrlen) == -1) {
            close(sockfd);
            fprintf(stderr, "%s: bind: %s\n", progname, strerror(errno));
            continue;
        }

        break;
    }
    freeaddrinfo(servinfo);


    if (iter == NULL)  {
        fprintf(stderr, "%s: failed to bind\n", progname);
        return NULL;
    }

    if (listen(sockfd, queuesize) == -1) {
        fprintf(stderr, "%s: listen: %s\n", progname, strerror(errno));
        return NULL;
    }


    socket_t *socket = xmalloc(sizeof(socket_t));
    socket->fd = sockfd;
    return socket;
}
