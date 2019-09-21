#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#include "utils.h"
#include "socket.h"

static int version(int status) {
  fprintf(status ? stderr : stdout, "lollek/slowproc v0.1-2019.09.21\n");
  return status;
}

static int usage(int status) {
    fprintf(status ? stderr : stdout,
            "Usage: %s [OPTIONS] HOSTNAME PORT\n"
            "Opens a socket on which it forwards data to HOSTNAME at PORT.\n"
            "The sockets' input and output will be connected until either hangs up.\n\n",
            progname);
    fprintf(status ? stderr : stdout,
            "  -b, --bind=PORT          start listening on PORT (default 8080)\n"
            "  -q, --queuesize=N        Socket queue size (default 100)\n"
            "      --help               display this help and exit\n"
            "      --version            output version information and exit\n\n");
    return version(status);
}

int main(int argc, char *argv[]) {
    progname = argv[0];

    int option_index = 0;
    struct option const long_options[] = {
        {"bind",             required_argument, 0, 'b'},
        {"queuesize",        required_argument, 0, 'q'},
        {"help",             no_argument,       0,  0 },
        {"version",          no_argument,       0,  1 },
        {0,                  0,                 0,  0 }
    };

    char const *listen_port = "8080";
    int queuesize = 100;

    while (1) {
        int c = getopt_long(argc, argv, "b:hq:v",
                long_options, &option_index);
        if (c == -1) {
            break;
        }
        switch (c) {
            case 'b': listen_port = optarg; break;
            case 'q': queuesize = strtol(optarg, NULL, 10); break;
            case  0: return usage(0);
            case  1: return version(0);
            default:
                     fprintf(stderr, "Try '%s --help' for more information\n",
                             progname);
                     return 1;;
        }
    }

    if (argc - optind != 2) {
        return usage(1);
    }

    char const *target_hostname = argv[optind];
    char const *target_port = argv[optind + 1];

    socket_t *listen_server = socket_create(NULL, listen_port, queuesize);
    if (listen_server == NULL) {
        return 1;
    }

    printf("Listening on port %s\n", listen_port);
    printf("Target is %s:%s\n", target_hostname, target_port);

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
