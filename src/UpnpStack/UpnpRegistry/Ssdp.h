/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   Ssdp.h
*
* @remark
*
*/

#ifndef __SSDP_H__
#define __SSDP_H__

#include "tiny_base.h"
#include "TinyThread.h"
#include "TinySelector.h"
#include "TinySocketIpc.h"
#include "SsdpMessage.h"

TINY_BEGIN_DECLS


typedef void(*SsdpMessageHandler)(SsdpMessage *message, void *ctx);

typedef struct _Ssdp
{
    TinyThread                  thread;
    TinySelector                selector;
    TinySocketIpc               ipc;
    bool                        running;
    int                         group_fd;
    int                         search_fd;
    SsdpMessageHandler          handler;
    void                      * ctx;
} Ssdp;

Ssdp * Ssdp_New(void);
TinyRet Ssdp_Construct(Ssdp *thiz);
void Ssdp_Dispose(Ssdp *thiz);
void Ssdp_Delete(Ssdp *thiz);

TinyRet Ssdp_SetMessageHandler(Ssdp *thiz, SsdpMessageHandler handler, void *ctx);
TinyRet Ssdp_Start(Ssdp *thiz);
TinyRet Ssdp_Stop(Ssdp *thiz);
TinyRet Ssdp_SendMessage(Ssdp *thiz, SsdpMessage *message);


TINY_END_DECLS

#endif /* __SSDP_H__ */