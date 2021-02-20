#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "connection.h"

extern int errno;

int initiate_tcp_server(int port, int listen_queue_size) {
    int sockfd;
    struct sockaddr_in addr;

    if (port < 0 || listen_queue_size < 0) {
        syslog(LOG_ERR, "Invalid arguments");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Creating TCP socket...");
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        syslog(LOG_ERR, "Error creating socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET; /* Familia TCP/IP */
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero((void*)&(addr.sin_zero), 8);

    syslog(LOG_INFO, "Binding socket...");
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        syslog(LOG_ERR, "Error binding socket: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Listening connections...");
    if (listen(sockfd, listen_queue_size) < 0) {
        syslog(LOG_ERR, "Error listening: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

void accept_connection(int sockfd, service_launcher_type launch_service) {
    int confd, conlen;
    struct sockaddr connection;

    if (sockfd < 0) {
        syslog(LOG_ERR, "Invalid socket descriptor");
        exit(EXIT_FAILURE);
    }

    conlen = sizeof(connection);

    if ((confd = accept(sockfd, &connection, &conlen)) < 0) {
        syslog(LOG_ERR, "Error accepting connection: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    launch_service(sockfd);
    wait();

    return;
}
