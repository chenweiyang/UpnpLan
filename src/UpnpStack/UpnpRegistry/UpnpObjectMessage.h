/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObjectMessage.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_OBJECT_MESSAGE_H__
#define __UPNP_OBJECT_MESSAGE_H__

#include "tiny_base.h"
#include "upnp_define.h"
#include "UpnpUsn.h"

TINY_BEGIN_DECLS

#define UPNP_OBJECT_MESSAGE_BUF_LEN    2048

typedef struct _UpnpObjectMessage
{
    char buf[UPNP_OBJECT_MESSAGE_BUF_LEN];
    char ip[TINY_IP_LEN];
    uint16_t port;
    int bytes_read;
} UpnpObjectMessage;

UpnpObjectMessage * UpnpObjectMessage_New(void);
TinyRet UpnpObjectMessage_Construct(UpnpObjectMessage *thiz);
TinyRet UpnpObjectMessage_Dispose(UpnpObjectMessage *thiz);
void UpnpObjectMessage_Delete(UpnpObjectMessage *thiz);

const char * UpnpObjectMessage_GetBuffer(UpnpObjectMessage *thiz);
const char * UpnpObjectMessage_GetIp(UpnpObjectMessage *thiz);
uint16_t UpnpObjectMessage_GetPort(UpnpObjectMessage *thiz);
int UpnpObjectMessage_GetBytesRead(UpnpObjectMessage *thiz);


TINY_END_DECLS


#endif /* __UPNP_OBJECT_MESSAGE_H__ */
