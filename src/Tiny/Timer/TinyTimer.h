/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyTimer.h
 *
 * @remark
 *
 */

#ifndef __TINY_TIMER_H__
#define __TINY_TIMER_H__

#include "tiny_base.h"
#include "TinySocketIpc.h"
#include "TinyThread.h"

TINY_BEGIN_DECLS

struct _TinyTimer;
typedef struct _TinyTimer TinyTimer;

typedef bool(*TinyTimerListener)(TinyTimer *thiz, void *ctx);

struct _TinyTimer
{
    bool                    is_running;
    uint32_t                times;
    uint64_t                interval;
    uint64_t                running_interval;
    TinyTimerListener         listener;
    void                  * listener_ctx;
    TinyThread                thread;
    TinySocketIpc             ipc;
};

TinyTimer * TinyTimer_New(void);
TinyRet TinyTimer_Construct(TinyTimer *thiz);
TinyRet TinyTimer_Initialize(TinyTimer *thiz, uint64_t interval, uint32_t times);
TinyRet TinyTimer_Dispose(TinyTimer *thiz);
void TinyTimer_Delete(TinyTimer *thiz);

TinyRet TinyTimer_Start(TinyTimer *thiz, TinyTimerListener listener, void *ctx);
TinyRet TinyTimer_Stop(TinyTimer *thiz);


TINY_END_DECLS

#endif /* __MI_TIMER_H__ */
