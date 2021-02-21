#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include "connection.h"

extern int errno;
static pid_t *children_pid = NULL; /* Almacena los pid de los procesos hijo */
static sem_t *fork_sem; /* Semaforo para accept_connections_fork */

/* Funciones Privadas */
void sig_int(int signo); /* handler de SIGINT */

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

void accept_connections_fork(int sockfd, service_launcher_type launch_service,
        int max_children) {
    int confd, conlen;
    int n_children = 0;
    struct sockaddr connection;

    if (sockfd < 0) {
        syslog(LOG_ERR, "Invalid socket descriptor");
        exit(EXIT_FAILURE);
    }

    if (max_children < 1 || max_children > MAX_CHLD) {
        syslog(LOG_ERR, "Invalid maximum children number");
        exit(EXIT_FAILURE);
    }

    /* Semaforo para evitar aceptar mas de max_children conexiones */
    if ((fork_sem = sem_open("/fork_sem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,
                    max_children)) == SEM_FAILED) {
        syslog(LOG_ERR, "Error creating semaphore: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    sem_unlink("/fork_sem");

    conlen = sizeof(connection);

    for ( ; ; ) {
        sem_wait(fork_sem); /* Espera a que finalicen los max_children hijos */

        if ( (confd = accept(sockfd, &connection, (socklen_t*)&conlen)) < 0) {
            syslog(LOG_ERR, "Error accepting connection: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if ( (children_pid[n_children] = fork()) < 0) {
            syslog(LOG_ERR, "Error spawning child: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        if (children_pid[n_children] == 0) {
            launch_service(confd);
            sem_post(fork_sem); /* Libera al padre del sem_wait */
            sem_close(fork_sem);
            exit(EXIT_SUCCESS);
        }

        close(confd);
    }
}

/* Implementacion de Funciones Privadas */
void sig_int(int signo) {

}
