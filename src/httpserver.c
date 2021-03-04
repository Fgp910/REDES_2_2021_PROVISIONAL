#include <assert.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>

#include "connection.h"
#include "picohttpparser.h"

#define STR_LEN 128

extern int errno;

void http_parser_test(int sock, void *args) {
    char buf[4096];
    const char *method, *path;
    int pret, minor_version;
    struct phr_header headers[100];
    size_t buflen = 0, prevbuflen = 0, method_len, path_len, num_headers;
    ssize_t rret;
    int i;

    while (1) {
        fprintf(stdout, "Processing HTTP request...\n");
        /* read the request */
        while ( (rret = recv(sock, buf + buflen, sizeof(buf) - buflen, 0)) == -1
                && errno == EINTR);

        if (rret <= 0)
            return ;
        prevbuflen = buflen;
        buflen += rret;
        /* parse the request */
        num_headers = sizeof(headers) / sizeof(headers[0]);
        pret = phr_parse_request(buf, buflen, &method, &method_len, &path,
                &path_len, &minor_version, headers, &num_headers, prevbuflen);
        if (pret > 0)
            break; /* successfully parsed the request */
        else if (pret == -1)
            return ;
        /* request is incomplete, continue the
         * loop */
        assert(pret == -2);
        if (buflen == sizeof(buf))
            return ;
    }

    printf("request is %d bytes long\n", pret);
    printf("method is %.*s\n", (int)method_len, method);
    printf("path is %.*s\n", (int)path_len, path);
    printf("HTTP version is 1.%d\n", minor_version);
    printf("headers:\n");
    for (i = 0; i != num_headers; ++i) {
        printf("%.*s: %.*s\n", (int)headers[i].name_len, headers[i].name,
                (int)headers[i].value_len, headers[i].value);
    }
}

int main() {
    int listenfd;

    listenfd = initiate_tcp_server(8080, 1, 0);

    accept_connections(listenfd, http_parser_test, NULL);

    exit(EXIT_SUCCESS);
}
