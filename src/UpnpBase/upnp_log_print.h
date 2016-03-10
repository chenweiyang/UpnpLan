/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   upnp_log_print.h
 *
 * @remark
 *
 */

#ifndef __UPNP_LOG_PRINT_H__
#define __UPNP_LOG_PRINT_H__

#include <stdarg.h>
#include "upnp_macro.h"

UPNP_BEGIN_DECLS


int __upnp_log_open(const char *log_file);

int __upnp_log_close(void);

/*
 * log priority values, in ascending priority order.
 */
typedef enum _upnp_log_priority {
    UPNP_LOG_UNKNOWN = 0,
    UPNP_LOG_VERBOSE,
    UPNP_LOG_DEBUG,
    UPNP_LOG_INFO,
    UPNP_LOG_WARN,
    UPNP_LOG_ERROR,
} upnp_log_priority;

/*
 * Send a simple string to the log.
 */
int __upnp_log_write(int prio, const char *tag, const char *text);

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
#if defined(__GNUC__)
int __upnp_log_print(int prio, const char *tag, const char *fmt, ...) __attribute__((format(printf, 3, 4)));
#else
int __upnp_log_print(int prio, const char *tag, const char *fmt, ...);
#endif

/*
 * A variant of __upnp_log_print() that takes a va_list to list
 * additional parameters.
 */
int __upnp_log_vprint(int prio, const char *tag, const char *fmt, va_list ap);


UPNP_END_DECLS

#endif /* __UPNP_LOG_PRINT_H__ */
