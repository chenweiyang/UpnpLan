/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   TinyMulticast.h
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#ifndef __TINY_MULTICAST_H__
#define __TINY_MULTICAST_H__

#include "tiny_base.h"
#include "TinyList.h"
#include "TinyMulticastSocket.h"

TINY_BEGIN_DECLS

#define TINY_MULTICAST_MAX  32

typedef struct _TinyMulticast
{
    TinyList list;
    char group[TINY_IP_LEN];
    uint16_t port;
    bool block;
} TinyMulticast;

TinyMulticast * TinyMulticast_New(void);
TinyRet TinyMulticast_Construct(TinyMulticast *thiz);
void TinyMulticast_Dispose(TinyMulticast *thiz);
void TinyMulticast_Delete(TinyMulticast *thiz);

TinyRet TinyMulticast_Open(TinyMulticast *thiz, const char *group, uint16_t port, bool block);
TinyRet TinyMulticast_Close(TinyMulticast *thiz);

uint32_t TinyMulticast_GetCount(TinyMulticast *thiz);
TinyMulticastSocket * TinyMulticast_GetSocketAt(TinyMulticast *thiz, uint32_t index);


TINY_END_DECLS

#endif /* __TINY_MULTICAST_H__ */