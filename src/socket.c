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
#include <sys/param.h>

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

void socket_send(socket_t const *const socket, char const *const data) {
    int const datasize = strlen(data);
    int const status = send(socket->fd, data, datasize, 0);
    if (status == -1) {
        fprintf(stderr, "%s: accept: %s\n", progname, strerror(errno));
    }
}

socket_t *socket_accept(socket_t const *const socket) {
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

    socket_t *conn = xmalloc(sizeof(socket_t));
    conn->fd = fd;
    strcpy(conn->ipaddr, ipaddr_str);
    return conn;
}

socket_t *socket_create(char const *restrict hostname, char const *restrict port, int queuesize) {
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
    int const status = getaddrinfo(hostname, port, &hints, &servinfo);
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

        if (hostname == NULL) {
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
        } else {
            if (connect(sockfd, iter->ai_addr, iter->ai_addrlen) == -1) {
                close(sockfd);
                fprintf(stderr, "%s: connect: %s\n", progname, strerror(errno));
                continue;
            }
        }

        break;
    }
    freeaddrinfo(servinfo);

    if (iter == NULL)  {
        fprintf(stderr, "%s: failed to bind\n", progname);
        return NULL;
    }

    if (hostname == NULL) {
        if (listen(sockfd, queuesize) == -1) {
            fprintf(stderr, "%s: listen: %s\n", progname, strerror(errno));
            return NULL;
        }
    }

    socket_t *socket = xmalloc(sizeof(socket_t));
    socket->fd = sockfd;
    return socket;
}

void socket_join(socket_t *first, socket_t *second) {
    fd_set sockets;

    FD_SET(first->fd, &sockets);
    FD_SET(second->fd, &sockets);

    while (1) {
        fd_set read_fds = sockets;
        int fd_max = MAX(first->fd, second->fd) + 1;
        if (select(fd_max, &read_fds, NULL, NULL, NULL) == -1) {
            fprintf(stderr, "%s: select: %s\n", progname, strerror(errno));
            return;
        }

        char buf[4096];
        int indata_size = recv(first->fd, buf, sizeof(buf), 0);
        if (indata_size == 0) {
            printf("Socket hung up\n");
            return;
        }

        if (indata_size > 0) {
            if (send(second->fd, buf, indata_size, 0) == -1) {
                fprintf(stderr, "%s: send: %s\n", progname, strerror(errno));
            }
        }

        indata_size = recv(second->fd, buf, sizeof(buf), 0);
        if (indata_size == 0) {
            printf("Socket hung up\n");
            return;
        }

        if (indata_size > 0) {
            if (send(first->fd, buf, indata_size, 0) == -1) {
                fprintf(stderr, "%s: send: %s\n", progname, strerror(errno));
            }
        }
    }
}
