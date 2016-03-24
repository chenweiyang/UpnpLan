/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSubscriber.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_SUBSCRIBER_H__
#define __UPNP_SUBSCRIBER_H__

#include "tiny_base.h"
#include "UpnpError.h"
#include "UpnpSubscription.h"
#include "UpnpHttpManager.h"
#include "TinyMap.h"
#include "TinyMutex.h"

TINY_BEGIN_DECLS


typedef struct _UpnpSubscriber
{
    UpnpHttpManager           * http;
    TinyMap                     map;
    TinyMutex                   mutex;
} UpnpSubscriber;

UpnpSubscriber * UpnpSubscriber_New(UpnpHttpManager *http);
TinyRet UpnpSubscriber_Construct(UpnpSubscriber *thiz, UpnpHttpManager *http);
void UpnpSubscriber_Dispose(UpnpSubscriber *thiz);
void UpnpSubscriber_Delete(UpnpSubscriber *thiz);

TinyRet UpnpSubscriber_Subscribe(UpnpSubscriber *thiz, UpnpService *service, uint32_t timeout, UpnpEventListener listener, void *ctx, UpnpError *error);
TinyRet UpnpSubscriber_Unsubscribe(UpnpSubscriber *thiz, UpnpService *service, UpnpError *error);


TINY_END_DECLS

#endif /* __UPNP_SUBSCRIBER_H__ */
