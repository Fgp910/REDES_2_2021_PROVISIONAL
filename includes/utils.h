#ifndef UTILS_H
#define UTILS_H

#include <signal.h>

#define INFO 0
#define ERR 1
#define STD 0
#define LOG 1

/**
 * Prototipo de manejador de sennal.
 *
 * Descripcion: Define el prototipo de manejador de sennal. Es el tipo de
 * argumento que recibe set_sig_int.
 *
 * Argumentos:
 *  El numero de sennal.
 */
typedef void (*handler_t)(int);

/**
 * block_sigint
 *
 * Descripcion: Bloquea la sennal SIGINT.
 *
 * Argumentos:
 *  oldset - si es no nulo, almacena la mascara de sennales del hilo antes de la
 *  llamada.
 *
 * Salida:
 *  0 en caso de exito, -1 en otro caso.
 */
int block_sigint(sigset_t *oldset);

/**
 * set_sig_int
 *
 * Descripcion: Define el manejador de SIGINT.
 *
 * Argumentos:
 *  handler - el manejador a establecer para el tratamiento de SIGINT
 *
 * Salida:
 *  0 en caso de exito, -1 en otro caso.
 */
int set_sig_int(handler_t handler);

/**
 * set_logger_type
 *
 * Descripcion: Establece la salida del logger.
 *
 * Argumentos:
 *  type - el tipo de logger, STD para salida estandar (opcion por defecto), LOG
 *  para syslog.
 */
void set_logger_type(int type);

/**
 * logger
 *
 * Descripcion: Envia la cadena con formato a la salida establecida previamente
 * por set_logger_type.
 *
 * Argumentos:
 *  out - la salida especifica, INFO para la salida estandar, ERR para la salida
 *  de errores.
 *  format - la cadena de caracteres a enviar.
 *  args - los argumentos opcionales de formato de format.
 */
void logger(int out, const char *format, ...);

/**
 * daemonize
 *
 * Descripcion: Crea un proceso demonio. A efectos practicos, el proceso que la
 * llama se "demoniza".
 */
void daemonize();

#endif
