/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyMutex.h
 *
 * @remark
 *    set tabstop=4
 *    set shiftwidth=4
 *    set expandtab
 */

#ifndef __TINY_MUTEX_H__
#define __TINY_MUTEX_H__

#ifdef _WIN32
#else
#include <pthread.h>
#endif

#include "tiny_base.h"

TINY_BEGIN_DECLS

#ifdef _WIN32
    typedef CRITICAL_SECTION    ct_mutex_t;
#else /* Linux */
    typedef pthread_mutex_t     ct_mutex_t;
#endif /* _WIN32 */

typedef struct _TinyMutex
{
    ct_mutex_t    mutex;
} TinyMutex;

TinyMutex * TinyMutex_New(void);
TinyRet TinyMutex_Construct(TinyMutex *thiz);
TinyRet TinyMutex_Dispose(TinyMutex *thiz);
void TinyMutex_Delete(TinyMutex *thiz);

bool TinyMutex_Lock(TinyMutex *thiz);
bool TinyMutex_Unlock(TinyMutex *thiz);


TINY_END_DECLS

#endif /* __TINY_MUTEX_H__ */
