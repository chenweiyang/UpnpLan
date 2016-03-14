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
#include "upnp_api.h"
#include "UpnpSubscription.h"
#include "UpnpError.h"

TINY_BEGIN_DECLS


struct _UpnpSubscriber;
typedef struct _UpnpSubscriber UpnpSubscriber;

UPNP_API UpnpSubscriber * UpnpSubscriber_New(void);
UPNP_API void UpnpSubscriber_Delete(UpnpSubscriber *thiz);

UPNP_API TinyRet UpnpSubscriber_Subscribe(UpnpSubscriber *thiz, UpnpSubscription *subscription, UpnpError *error);
UPNP_API TinyRet UpnpSubscriber_Unsubscribe(UpnpSubscriber *thiz, UpnpError *error);


TINY_END_DECLS

#endif /* __UPNP_SUBSCRIBER_H__ */
