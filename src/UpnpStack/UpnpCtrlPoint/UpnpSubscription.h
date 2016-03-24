/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSubscription.h
*
* @remark
*
*/

#ifndef __UPNP_SUBSCRIPTION_H__
#define __UPNP_SUBSCRIPTION_H__

#include "tiny_base.h"
#include "UpnpEvent.h"
#include "UpnpService.h"

TINY_BEGIN_DECLS


typedef void(*UpnpEventListener)(UpnpEvent *event, void *ctx);

typedef struct _UpnpSubscription
{
    UpnpService *service;
    UpnpEventListener listener;
    void *ctx;
    uint16_t port;
    uint32_t timeout;
    char subscribeId[UPNP_UUID_LEN];
    char callbackUri[TINY_URL_LEN];
    char callbackFull[TINY_URL_LEN];
} UpnpSubscription;

UpnpSubscription * UpnpSubscription_New(UpnpService *service, uint16_t port, uint32_t timeout, UpnpEventListener listener, void *ctx);
TinyRet UpnpSubscription_Construct(UpnpSubscription *thiz, UpnpService *service, uint16_t port, uint32_t timeout, UpnpEventListener listener, void *ctx);
void UpnpSubscription_Dispose(UpnpSubscription *thiz);
void UpnpSubscription_Delete(UpnpSubscription *thiz);

const char * UpnpSubscription_GetCallBackUri(UpnpSubscription *thiz);
const char * UpnpSubscription_GetFullCallBack(UpnpSubscription *thiz);


TINY_END_DECLS

#endif /* __UPNP_SUBSCRIPTION_H__ */
