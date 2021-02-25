#ifndef CONNECTION_H
#define CONNECTION_H

#define MAX_CHLD 32
#define MAX_THRD 32

/**
 * Prototipo de funcion de servicio.
 *
 * Descripcion: El proceso que la llama realiza el servicio. Es ejecutada
 * tipicamente por los procesos hijos. Planeada para ser argumento de
 * accept_connections(). Es importante que el proceso que ejecuta esta funcion
 * no finalice su ejecucion dentro de ella.
 *
 * Argumentos:
 *  El descriptor del socket de conexion.
 */
typedef void (*service_launcher_type)(int);

/**
 * initiate_tcp_server
 *
 * Descripcion: Inicia un servidor TCP. Abre un socket TCP, lo asocia al puerto
 * (bind) y queda a la escucha de conexiones (listen).
 *
 * Argumentos:
 *  port - el puerto de escucha del servidor.
 *  listen_queue_size - tamano de la cola de peticiones en espera.
 *
 * Salida:
 *  El descriptor del socket creado.
 */
int initiate_tcp_server(int port, int listen_queue_size);

/**
 * accept_connections
 *
 * Descripcion: Extrae conexiones de la cola de peticiones pendientes del socket
 * de escucha, crea un socket con las propiedades de sockfd y lanza el servicio
 * correspondiente. Es bloqueande en caso de no haber conexiones pendientes.
 * Acepta y atiende las conexiones de forma iterativa.
 *
 * Argumentos:
 *  sockfd - el descriptor del socket de escucha.
 *  launch_service - el lanzador del servicio a realizar.
 */
void accept_connections(int sockfd, service_launcher_type launch_service);

/**
 * accept_connections_fork
 *
 * Descripcion: Extrae conexiones de la cola de peticiones pendientes del socket
 * de escucha, crea un socket con las propiedades de sockfd y lanza el servicio
 * correspondiente. Es bloqueande en caso de no haber conexiones pendientes.
 * El proceso principal genera un proceso hijo por cada peticion que atiende.
 *
 * Argumentos:
 *  sockfd - el descriptor del socket de escucha.
 *  launch_service - el lanzador del servicio a realizar.
 *  max_children - el numero maximo de procesos hijos a generar.
 */
void accept_connections_fork(int sockfd, service_launcher_type launch_service,
        int max_children);

/**
 * accept_connections_thread
 *
 * Descripcion: Extrae conexiones de la cola de peticiones pendientes del socket
 * de escucha, crea un socket con las propiedades de sockfd y lanza el servicio
 * correspondiente. Es bloqueande en caso de no haber conexiones pendientes.
 * El proceso principal genera un hilo por cada peticion que atiende.
 *
 * Argumentos:
 *  sockfd - el descriptor del socket de escucha.
 *  launch_service - el lanzador del servicio a realizar.
 *  max_threads - el numero maximo de hilos a generar.
 */
void accept_connections_thread(int sockfd, service_launcher_type launch_service,
        int max_threads);

/**
 * accept_connections_pool_thread
 *
 * Descripcion: Extrae conexiones de la cola de peticiones pendientes del socket
 * de escucha, crea un socket con las propiedades de sockfd y lanza el servicio
 * correspondiente. Es bloqueande en caso de no haber conexiones pendientes.
 * El proceso principal genera un pool hilos que esperan y atienden las
 * peticiones.
 *
 * Argumentos:
 *  sockfd - el descriptor del socket de escucha.
 *  launch_service - el lanzador del servicio a realizar.
 *  n_threads - el numero de hilos a generar.
 */
void accept_connections_pool_thread(int sockfd, service_launcher_type
        launch_service, int n_threads);

#endif
