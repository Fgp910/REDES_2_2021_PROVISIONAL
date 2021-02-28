#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "connection.h"
#include "utils.h"

extern int errno;
static volatile sig_atomic_t sig_flag = 0; /* Almacena la ultima sennal */
static pid_t *children_pid = NULL; /* Almacena los pid de los procesos hijo */
static sem_t *fork_sem; /* Semaforo para accept_connections_fork */
static sem_t *thread_sem; /* Semaforo para accept_connections_thread */
static pthread_mutex_t pool_thread_mutex; /* Semaforo del pool de hilos */

/* Almacena la informacion necesaria para que un hilo atienda un cliente */
typedef struct {
    int sockfd; /* Descriptor del socket de escucha o conexion */
    service_launcher_t launch_service; /* Servicio a realizar */
} thread_args;


/* Funciones Privadas */
void sig_int(int signo);             /* Handler de SIGINT */
void stop_server_fork(int status);   /* Detiene el servidor multiproceso */
void thread_serve(void *args);       /* Funcion para hilos de servicio */
void stop_server_thread(int sockfd); /* Detiene el servidor multihilo */
void pool_thread_serve(void *args);  /* Funcion para hilos del pool */
void stop_server_pool_thread(int sockfd, pthread_t *tid, int n_threads,
        int status);                 /* Detiene el pool de hilos */

int initiate_tcp_server(int port, int listen_queue_size, int daemon) {
    int sockfd;
    struct sockaddr_in addr;

    if (port < 0 || listen_queue_size < 0 || (daemon != 0 && daemon != 1)) {
        logger(ERR, "Invalid arguments\n");
        return -1;
    }

    if (daemon) {
        set_logger_type(LOG);
        daemonize();
    } else {
        set_logger_type(STD);
    }

    logger(INFO, "Creating TCP socket...\n");
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        logger(ERR, "Error creating socket: %s\n", strerror(errno));
        return -1;
    }

    addr.sin_family = AF_INET; /* Familia TCP/IP */
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bzero((void*)&(addr.sin_zero), 8);

    logger(INFO, "Binding socket...\n");
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        logger(ERR, "Error binding socket: %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    logger(INFO, "Listening connections...\n");
    if (listen(sockfd, listen_queue_size) < 0) {
        logger(ERR, "Error listening: %s\n", strerror(errno));
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void accept_connections(int sockfd, service_launcher_t launch_service) {
    int confd, conlen;
    struct sockaddr connection;

    if (sockfd < 0) {
        logger(ERR, "Invalid socket descriptor\n");
        exit(EXIT_FAILURE);
    }

    conlen = sizeof(connection);

    for ( ; ; ) {
        if ( (confd = accept(sockfd, &connection, (socklen_t*)&conlen)) < 0) {
            logger(ERR, "Error accepting connection: %s\n", strerror(errno));
        }

        launch_service(confd);
        close(confd);
    }
}

void accept_connections_fork(int sockfd, service_launcher_t launch_service,
        int max_children) {
    int confd, conlen;
    int i = 0;
    struct sockaddr connection;
    pid_t pid;

    if (sockfd < 0) {
        logger(ERR, "Invalid socket descriptor\n");
        exit(EXIT_FAILURE);
    }

    if (max_children < 1 || max_children > MAX_CHLD) {
        logger(ERR, "Invalid maximum children number\n");
        exit(EXIT_FAILURE);
    }

    /* Memoria para el array de pids */
    children_pid = (pid_t*)malloc(max_children*sizeof(pid_t));
    if (!children_pid) {
        logger(ERR, "Error allocating memory\n");
        exit(EXIT_FAILURE);
    }

    /*Comportamiento ante SIGINT */
    if (set_sig_int(sig_int) < 0) {
        logger(ERR, "Error setting SIGINT handler: %s\n", strerror(errno));
        free(children_pid);
        exit(EXIT_FAILURE);
    }

    /* Semaforo para evitar aceptar mas de max_children conexiones */
    if ((fork_sem = sem_open("/fork_sem", O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,
                    max_children)) == SEM_FAILED) {
        logger(ERR, "Error creating semaphore: %s\n", strerror(errno));
        free(children_pid);
        exit(EXIT_FAILURE);
    }
    sem_unlink("/fork_sem");

    conlen = sizeof(connection);

    for ( ; ; ) {
        sem_wait(fork_sem); /* Espera a que finalicen los max_children hijos */
        if (sig_flag == SIGINT) stop_server_fork(EXIT_SUCCESS);

        /* Asignacion de una posicion en el array */
        for (i = 0; i < max_children; i++)
            if (children_pid[i] == 0) break;

        if ( (confd = accept(sockfd, &connection, (socklen_t*)&conlen)) < 0) {
            if (sig_flag == SIGINT)
                break;
            logger(ERR, "Error accepting connection: %s\n", strerror(errno));
            stop_server_fork(EXIT_FAILURE);
        }

        if ( (pid = fork()) < 0) {
            logger(ERR, "Error spawning child: %s\n", strerror(errno));
            close(confd);
            stop_server_fork(EXIT_FAILURE);
        }

        else if (pid == 0) {
            launch_service(confd);
            sem_post(fork_sem); /* Libera al padre del sem_wait */
            sem_close(fork_sem);
            children_pid[i] = 0;
            exit(EXIT_SUCCESS);
        }

        else
            children_pid[i] = pid;

        close(confd);
    }

    for (i = 0; i < max_children; i++) {
        if (children_pid[i] > 0) {
            if (kill(children_pid[i], SIGKILL) == -1)
                logger(ERR, "Error killing child process\n");
        }
    }
    stop_server_fork(EXIT_SUCCESS);
}

void accept_connections_thread(int sockfd, service_launcher_t launch_service,
        int max_threads) {
    int confd, conlen;
    pthread_t tid;
    struct sockaddr connection;

    if (sockfd < 0) {
        logger(ERR, "Invalid socket descriptor\n");
        exit(EXIT_FAILURE);
    }

    if (max_threads < 1 || max_threads > MAX_THRD) {
        logger(ERR, "Invalid maximum threads number\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (set_sig_int(sig_int) < 0) {
        logger(ERR, "Error setting SIGINT handler: %s\n", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    /* Semaforo para evitar aceptar mas de max_threads conexiones */
    if ( (thread_sem = sem_open("/thread_sem", O_CREAT | O_EXCL, S_IRUSR |
                    S_IWUSR, max_threads)) == SEM_FAILED) {
        logger(ERR, "Error creating semaphore: %s\n", strerror(errno));
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
            if (sig_flag == SIGINT) {
                stop_server_thread(sockfd);
            }
            logger(ERR, "Error accepting connection: %s\n", strerror(errno));
        } else {
            args.sockfd = confd;
            args.launch_service = launch_service;
            if (pthread_create(&tid, NULL, (void *)&thread_serve, (void *)&args))
            {
                logger(ERR, "Error creating thread: %s\n", strerror(errno));
            }
        }
    }
}

void accept_connections_pool_thread(int sockfd, service_launcher_t
        launch_service, int n_threads) {
    int i;
    pthread_t tid[MAX_THRD];
    sigset_t oldset;
    thread_args args;

    if (sockfd < 0) {
        logger(ERR, "Invalid socket descriptor\n");
        exit(EXIT_FAILURE);
    }

    if (n_threads < 1 || n_threads > MAX_THRD) {
        logger(ERR, "Invalid maximum threads number\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (block_sigint_thread(&oldset) < 0) {
        logger(ERR, "Error blocking SIGINT: %s\n", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (set_sig_int(sig_int) < 0) {
        logger(ERR, "Error setting SIGINT handler: %s\n", strerror(errno));
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&pool_thread_mutex, NULL)) {
        logger(ERR, "Error creating thread mutex\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    args.sockfd = sockfd;
    args.launch_service = launch_service;

    for (i = 0 ; i < n_threads ; i++) {
        if (pthread_create(&tid[i], NULL, (void *)&pool_thread_serve,
                    (void *)&args)) {
            logger(ERR, "Error creating thread: %s\n", strerror(errno));
            stop_server_pool_thread(sockfd, tid, i, EXIT_FAILURE);
        }
    }

    sigsuspend(&oldset); /* Espera a recibir la orden de parar el pool */
                         /* (por simplicidad, cualquier sennal vale)*/
    stop_server_pool_thread(sockfd, tid, n_threads, EXIT_SUCCESS);
}

/* Implementacion de Funciones Privadas */
void sig_int(int signo) { sig_flag = SIGINT; }

void stop_server_fork(int status) {
    free(children_pid);
    sem_close(fork_sem);
    exit(status);
}

void thread_serve(void *args) {
    thread_args *cast = (thread_args *)args;

    if (block_sigint_thread(NULL) < 0) {
        logger(ERR, "Thread: Error blocking SIGINT: %s\n", strerror(errno));
    }

    if (pthread_detach(pthread_self())) {
        logger(ERR, "Thread: Error detaching self: %s\n", strerror(errno));
    } else {
        cast->launch_service(cast->sockfd);
        close(cast->sockfd);
    }

    sem_post(thread_sem);
    pthread_exit(NULL);
}

void stop_server_thread(int sockfd) {
    logger(INFO, "\nStopping server...\n");
    close(sockfd);      /* thread_sem ya fue desligado, con lo que se borra al */
    exit(EXIT_SUCCESS); /* finalizar los hilos junto al proceso */
}

void pool_thread_serve(void *args) {
    thread_args *cast = (thread_args *)args;
    int confd, conlen;
    struct sockaddr connection;

    conlen = sizeof(connection);

    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    for ( ; ; ) { /* Solo abortamos los hilos cuando esperan conexiones */
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_mutex_lock(&pool_thread_mutex);
        if ( (confd = accept(cast->sockfd, &connection, (socklen_t*)&conlen)) < 0) {
            logger(ERR, "Thread: Error accepting connection: %s\n",
                    strerror(errno));
        }
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        pthread_mutex_unlock(&pool_thread_mutex);

        cast->launch_service(confd);
        close(confd);
    }
}

void stop_server_pool_thread(int sockfd, pthread_t *tid, int n_threads,
        int status) {
    int i;

    logger(INFO, "\nStopping server...\n");

    for (i = 0; i < n_threads; i++) { /* Cancelamos todos los hilos... */
        pthread_cancel(tid[i]);
    }

    for (i = 0; i < n_threads; i++) { /* ... y esperamos a que se unan */
        pthread_join(tid[i], NULL);
    }

    pthread_mutex_destroy(&pool_thread_mutex);
    close(sockfd);
    exit(status);
}
