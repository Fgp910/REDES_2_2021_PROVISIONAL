#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

#include "utils.h"

static volatile int logger_type = 0;

int block_sigint(sigset_t *oldset) {
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    return sigprocmask(SIG_BLOCK, &set, oldset);
}

int block_sigint_thread(sigset_t *oldset) {
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    return pthread_sigmask(SIG_BLOCK, &set, oldset);
}

int set_sig_int(handler_t handler) {
    struct sigaction s_int;

    s_int.sa_handler = handler;
    s_int.sa_flags = 0;
    sigemptyset(&(s_int.sa_mask));

    return sigaction(SIGINT, &s_int, NULL);
}

void set_logger_type(int type) {
    switch(type) {
        case STD:
        case LOG:
            logger_type = type;
            break;

        default:
            return;
    }
}

void logger(int out, const char *format, ...) {
    va_list args;
    va_start (args, format);

    switch(logger_type) {
        case STD:
            if (out == INFO) {
                vfprintf(stdout, format, args);
            } else if(out == ERR) {
                vfprintf(stderr, format, args);
            }
            break;

        case LOG:
            if (out == INFO) {
                syslog(LOG_INFO, format, args);
            } else if(out == ERR) {
                syslog(LOG_ERR, format, args);
            }
            break;
    }
    va_end(args);
}

void daemonize() {
    pid_t pid;

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); /* El padre termina */

    umask(0);
    setlogmask(LOG_UPTO(LOG_INFO));
    openlog("Server system messages:", LOG_CONS | LOG_PID | LOG_NDELAY,
            LOG_LOCAL3);
    syslog(LOG_ERR, "Ininitating new server...");

    if (setsid() < 0) { /* Crea nuevo SID para el proceso hijo */
        logger(ERR, "Error creating a new SID for the child process\n");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) { /* Cambia el direciorio de trabajo actual */
        logger(ERR, "Error changing the current working directory = \"/\"\n");
        exit(EXIT_FAILURE);
    }

    syslog(LOG_INFO, "Closing standard file descriptors...");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}
