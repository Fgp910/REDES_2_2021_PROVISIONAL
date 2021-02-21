#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>
#include <unistd.h>
#include "connection.h"

#define STR_LEN 128

void my_launcher(int confd) {
    int nbytes;
    char auxstr[STR_LEN], resp[STR_LEN];

    syslog(LOG_INFO, "Processing request...");
    if ( (nbytes = recv(confd, &auxstr, (STR_LEN - 1)*sizeof(char), 0)) < 0) {
        syslog(LOG_ERR, "Error processing request");
        exit(EXIT_FAILURE);
    }
    auxstr[nbytes] = '\0';

    sprintf(resp, "\'%s\' has %ld characters.", auxstr, strlen(auxstr));
    if (send(confd, &resp, (strlen(resp) + 1)*sizeof(char), 0) < 0) {
        syslog(LOG_ERR, "Error sending result");
        exit(EXIT_FAILURE);
    }

    close(confd);
    syslog(LOG_INFO, "Exiting service...");
}

int main() {
    int listenfd;

    listenfd = initiate_tcp_server(8080, 1);

    accept_connections_fork(listenfd, my_launcher);

    exit(EXIT_SUCCESS);
}
