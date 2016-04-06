/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_HTTP_CLIENT_H__
#define __UPNP_HTTP_CLIENT_H__

#include "tiny_base.h"
#include "HttpMessage.h"
#include "HttpClient.h"
#include "UpnpAction.h"
#include "UpnpError.h"
#include "UpnpSubscription.h"

TINY_BEGIN_DECLS


typedef struct _UpnpHttpClient
{
    HttpClient  * client;
} UpnpHttpClient;

UpnpHttpClient * UpnpHttpClient_New(void);
TinyRet UpnpHttpClient_Construct(UpnpHttpClient *thiz);
void UpnpHttpClient_Dispose(UpnpHttpClient *thiz);
void UpnpHttpClient_Delete(UpnpHttpClient *thiz);

TinyRet UpnpHttpClient_Post(UpnpHttpClient *thiz, UpnpAction *action, UpnpError *error, uint32_t timeout);
TinyRet UpnpHttpClient_Notify(UpnpHttpClient *thiz, UpnpEvent *event);
TinyRet UpnpHttpClient_Subscribe(UpnpHttpClient *thiz, UpnpSubscription *subscription, UpnpError *error, uint32_t timeout);
TinyRet UpnpHttpClient_Unsubscribe(UpnpHttpClient *thiz, UpnpSubscription *subscription, UpnpError *error, uint32_t timeout);


TINY_END_DECLS

#endif /* __UPNP_HTTP_CLIENT_H__ */