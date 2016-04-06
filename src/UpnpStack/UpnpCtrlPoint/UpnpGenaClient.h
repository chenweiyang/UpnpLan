/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpGenaClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_GENA_CLIENT_H__
#define __UPNP_GENA_CLIENT_H__

#include "tiny_base.h"
#include "UpnpError.h"
#include "UpnpSubscription.h"
#include "UpnpHttpManager.h"
#include "TinyMap.h"
#include "TinyMutex.h"

TINY_BEGIN_DECLS


typedef struct _UpnpGenaClient
{
    UpnpHttpManager           * http;
    TinyMap                     map;
    TinyMutex                   mutex;
} UpnpGenaClient;

UpnpGenaClient * UpnpGenaClient_New(UpnpHttpManager *http);
TinyRet UpnpGenaClient_Construct(UpnpGenaClient *thiz, UpnpHttpManager *http);
void UpnpGenaClient_Dispose(UpnpGenaClient *thiz);
void UpnpGenaClient_Delete(UpnpGenaClient *thiz);

TinyRet UpnpGenaClient_Subscribe(UpnpGenaClient *thiz, UpnpService *service, uint32_t timeout, UpnpEventListener listener, void *ctx, UpnpError *error);
TinyRet UpnpGenaClient_Unsubscribe(UpnpGenaClient *thiz, UpnpService *service, UpnpError *error);


TINY_END_DECLS

#endif /* __UPNP_GENA_CLIENT_H__ */
