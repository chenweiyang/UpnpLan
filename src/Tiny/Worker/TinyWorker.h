/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TinyWorker.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __TINY_WORKER_H__
#define __TINY_WORKER_H__

#include "tiny_base.h"
#include "TinyBlockingQueue.h"
#include "TinyThread.h"

TINY_BEGIN_DECLS


struct _TinyWorker;
typedef struct _TinyWorker TinyWorker;

typedef bool(*TinyWorkerListener)(TinyWorker *worker, void *job, void *ctx);
typedef void(*TinyWorkerJobDeleteListener)(TinyWorker *worker, void *job, void *ctx);

struct _TinyWorker
{
    bool                            is_running;
    TinyBlockingQueue                     job_queue;
    TinyWorkerListener                listener;
    void                          * listener_ctx;
    TinyThread                        thread;
    TinyWorkerJobDeleteListener       job_delete_listener;
    void                          * job_delete_listener_ctx;
};

TinyWorker * ScWorker_New(void);
TinyRet ScWorker_Construct(TinyWorker *thiz);
TinyRet ScWorker_Initialize(TinyWorker *thiz, TinyWorkerJobDeleteListener listener, void *ctx);
TinyRet ScWorker_Dispose(TinyWorker *thiz);
void ScWorker_Delete(TinyWorker *thiz);

const char * ScWorker_GetName(TinyWorker *thiz);
TinyRet ScWorker_Start(TinyWorker *thiz, const char *name, TinyWorkerListener listener, void *ctx);
TinyRet ScWorker_Stop(TinyWorker *thiz);
bool ScWorker_IsStarted(TinyWorker *thiz);

TinyRet ScWorker_PutJob(TinyWorker *thiz, void *job);
void * ScWorker_GetJob(TinyWorker *thiz);


TINY_END_DECLS

#endif /* __TINY_WORKER_H__ */
