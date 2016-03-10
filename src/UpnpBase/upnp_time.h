/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   upnp_time.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __UPNP_TIME_H__
#define __UPNP_TIME_H__

#include <stdint.h>
#include "upnp_macro.h"

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#else /* Linux */
#include <unistd.h>
#include <sys/time.h>
#endif /* _WIN32 */

UPNP_BEGIN_DECLS


#ifdef _WIN32
int gettimeofday(struct timeval *tv, void *tz);
#endif /* _WIN32 */

void upnp_sleep(int second);
void upnp_usleep(int usecond);
int upnp_getstrtime(char buf[], int len);

static UPNP_INLINE uint64_t upnp_getusec(void)
{
    struct timeval __tv__ = {0};
    return (gettimeofday(&__tv__, NULL) ? 0LL : 1000000LL * __tv__.tv_sec + __tv__.tv_usec); 
}


UPNP_END_DECLS

#endif /* __UPNP_TIME_H__ */
