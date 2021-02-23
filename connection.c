#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include "connection.h"

extern int errno;
static volatile sig_atomic_t sig_flag = 0; /* Almacena la ultima sennal */
static pid_t *children_pid = NULL; /* Almacena los pid de los procesos hijo */
static sem_t *fork_sem; /* Semaforo para accept_connections_fork */
static sem_t *thread_sem; /* Semaforo para accept_connections_thread */

/* Almacena la informacion necesaria para que un hilo atienda un cliente */
typedef struct {
    int confd; /* Descriptor del socket de conexion */
    service_launcher_type launch_service; /* Servicio a realizar */
} thread_args;

/* Funciones Privadas */
void sig_int(int signo);        /* Handler de SIGINT */
int set_sig_int();              /* Define sig_int como handler de SIGINT */
int block_sigint();             /* Bloquea SIGINT */
void thread_serve(void *args);  /* Funcion para hilos de servicio */
void stop_server_thread(int sockfd); /* Detiene el servidor multihilo */

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

void accept_connections_thread(int sockfd, service_launcher_type launch_service,
        int max_threads) {
    int confd, conlen;
    pthread_t tid;
    struct sockaddr connection;

    if (sockfd < 0) {
        syslog(LOG_ERR, "Invalid socket descriptor");
        exit(EXIT_FAILURE);
    }

    if (max_threads < 1 || max_threads > MAX_THRD) {
        syslog(LOG_ERR, "Invalid maximum threads number");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (set_sig_int() < 0) {
        syslog(LOG_ERR, "Error setting SIGINT handler: %s", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* Semaforo para evitar aceptar mas de max_threads conexiones */
    if ( (thread_sem = sem_open("/thread_sem", O_CREAT | O_EXCL, S_IRUSR |
                    S_IWUSR, max_threads)) == SEM_FAILED) {
        syslog(LOG_ERR, "Error creating semaphore: %s", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    sem_unlink("/thread_sem");

    conlen = sizeof(connection);

    for ( ; ; ) {
        thread_args args;

        sem_wait(thread_sem); /* Espera a que finalicen los max_threads hijos */
        if (sig_flag == SIGINT) stop_server_thread(sockfd);

        if ( (confd = accept(sockfd, &connection, (socklen_t*)&conlen)) < 0) {
            if (confd == EINTR && sig_flag == SIGINT) {
                stop_server_thread(sockfd);
            }
            syslog(LOG_ERR, "Error accepting connection: %s", strerror(errno));
        } else {
            args.confd = confd;
            args.launch_service = launch_service;
            if (pthread_create(&tid, NULL, (void *)&thread_serve, (void *)&args))
            {
                syslog(LOG_ERR, "Error creating thread: %s", strerror(errno));
            }
        }
    }
}

/* Implementacion de Funciones Privadas */
void sig_int(int signo) { sig_flag = SIGINT; }

int set_sig_int() {
    struct sigaction s_int;

    s_int.sa_handler = sig_int;
    s_int.sa_flags = 0;
    sigemptyset(&(s_int.sa_mask));

    return sigaction(SIGINT, &s_int, NULL);
}

int block_sigint() {
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    return sigprocmask(SIG_BLOCK, &set, NULL);
}

void thread_serve(void *args) {
    thread_args *cast = (thread_args *)args;

    if (block_sigint() < 0) {
        syslog(LOG_ERR, "Thread: Error blocking SIGINT: %s", strerror(errno));
    }

    if (pthread_detach(pthread_self())) {
        syslog(LOG_ERR, "Thread: Error detaching self: %s", strerror(errno));
    } else {
        cast->launch_service(cast->confd);
    }

    sem_post(thread_sem);
    pthread_exit(NULL);
}

void stop_server_thread(int sockfd) {
    syslog(LOG_INFO, "Stopping server...");
    close(sockfd);      /* thread_sem ya fue desligado, con lo que se borra al */
    exit(EXIT_SUCCESS); /* finalizar los hilos junto al proceso */
}
