/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyCondition.h
 *
 * @remark
 *
 */

#ifndef __TINY_CONDITION_H__
#define __TINY_CONDITION_H__

#include "tiny_base.h"
#include "TinyMutex.h"

TINY_BEGIN_DECLS


#ifdef _WIN32
typedef HANDLE              ct_cond_t;
#else /* Linux */
typedef pthread_cond_t      ct_cond_t;
#endif /* _WIN32 */

typedef struct _TinyCondition
{
    TinyMutex      mutex;
    ct_cond_t    job;
    bool         is_ready;
} TinyCondition;

TinyCondition * TinyCondition_New(void);
TinyRet TinyCondition_Construct(TinyCondition *thiz);
TinyRet TinyCondition_Dispose(TinyCondition *thiz);
void TinyCondition_Delete(TinyCondition *thiz);

bool TinyCondition_Wait(TinyCondition *thiz);
bool TinyCondition_NotifyOne(TinyCondition *thiz);
bool TinyCondition_NotifyAll(TinyCondition *thiz);


TINY_END_DECLS

#endif /* __TINY_CONDITION_H__ */
