#ifndef CONNECTION_H
#define CONNECTION_H

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
 *  El descriptor de fichero devuelto por la llamada a socket(), es decir, el
 *  descriptor del socket creado.
 */
int initiate_tcp_server(int port, int listen_queue_size);

/**
 * accept_connection
 *
 * Descripcion: Saca la primera conexion de la cola de peticiones pendientes
 * del socket de escucha, crea un socket con las propiedades de sockfd y lanza
 * el servicio correspondiente. Es bloqueante en caso de no haber conexiones
 * pendientes.
 *
 * Argumentos:
 *  sockfd - el descriptor del socket de escucha.
 */
void accept_connection(int sockfd);



#endif
