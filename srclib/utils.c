#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

#include "utils.h"

static volatile int logger_type = 0;

int block_sigint(sigset_t *oldset) {
    sigset_t set;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);

    return sigprocmask(SIG_BLOCK, &set, oldset);
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
