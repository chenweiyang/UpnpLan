/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   TinyMulticastSocket.h
*
* @remark
*    set tabstop=4
*    set shiftwidth=4
*    set expandtab
*/

#ifndef __TINY_MULTICAST_SOCKET_H__
#define __TINY_MULTICAST_SOCKET_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


typedef struct _TinyMulticastSocket
{
    char        ip[TINY_IP_LEN];
    int         fd;
} TinyMulticastSocket;

TinyMulticastSocket * TinyMulticastSocket_New(void);
TinyRet TinyMulticastSocket_Construct(TinyMulticastSocket *thiz);
void TinyMulticastSocket_Dispose(TinyMulticastSocket *thiz);
void TinyMulticastSocket_Delete(TinyMulticastSocket *thiz);

TinyRet TinyMulticastSocket_Open(TinyMulticastSocket *thiz, unsigned long ip, const char *group, uint16_t port, bool block);
TinyRet TinyMulticastSocket_Close(TinyMulticastSocket *thiz);


TINY_END_DECLS

#endif /* __TINY_MULTICAST_SOCKET_H__ */
