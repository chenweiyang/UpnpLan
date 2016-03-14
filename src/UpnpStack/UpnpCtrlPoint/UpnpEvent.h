/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpEvent.h
*
* @remark
*
*/

#ifndef __UPNP_EVENT_H__
#define __UPNP_EVENT_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "PropertyList.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


struct _UpnpEvent;
typedef struct _UpnpEvent UpnpEvent;

UPNP_API UpnpEvent * UpnpEvent_New(void);
UPNP_API void UpnpEvent_Delete(UpnpEvent *thiz);

UPNP_API TinyRet UpnpEvent_SetPropertyValue(UpnpEvent *thiz, const char *propertyName, const char *value);
UPNP_API const char * UpnpEvent_GetPropertyValue(UpnpEvent *thiz, const char *propertyName);

UPNP_API PropertyList* UpnpEvent_GetArgumentList(UpnpEvent *thiz);
UPNP_API TinyRet UpnpEvent_SetArgumentValue(UpnpEvent *thiz, const char *argumentName, const char *value);
UPNP_API const char * UpnpEvent_GetArgumentValue(UpnpEvent *thiz, const char *argumentName);

UPNP_API TinyRet UpnpEvent_Parse(UpnpEvent *thiz, HttpMessage *request);
UPNP_API TinyRet UpnpEvent_ToString(UpnpEvent *thiz, char *bytes, uint32_t len);


TINY_END_DECLS

#endif /* __UPNP_EVENT_H__ */
