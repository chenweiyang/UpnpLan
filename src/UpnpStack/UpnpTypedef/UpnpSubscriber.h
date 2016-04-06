/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSubscriber.h
*
* @remark
*
*/

#ifndef __UPNP_SUBSCRIBER_H__
#define __UPNP_SUBSCRIBER_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "upnp_define.h"

TINY_BEGIN_DECLS


struct _UpnpSubscriber;
typedef struct _UpnpSubscriber UpnpSubscriber;

UPNP_API UpnpSubscriber * UpnpSubscriber_New();
UPNP_API TinyRet UpnpSubscriber_Construct(UpnpSubscriber *thiz);
UPNP_API void UpnpSubscriber_Dispose(UpnpSubscriber *thiz);
UPNP_API void UpnpSubscriber_Delete(UpnpSubscriber *thiz);

UPNP_API TinyRet UpnpSubscriber_SetCallback(UpnpSubscriber *thiz, const char *callback);
UPNP_API const char * UpnpSubscriber_GetCallback(UpnpSubscriber *thiz);

UPNP_API TinyRet UpnpSubscriber_SetTimeout(UpnpSubscriber *thiz, uint32_t timeout);
UPNP_API uint32_t UpnpSubscriber_GetTimeout(UpnpSubscriber *thiz);

UPNP_API TinyRet UpnpSubscriber_SetSid(UpnpSubscriber *thiz, const char *sid);
UPNP_API const char * UpnpSubscriber_GetSid(UpnpSubscriber *thiz);


TINY_END_DECLS

#endif /* __UPNP_SUBSCRIBER_H__ */
