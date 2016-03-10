/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpAction.h
*
* @remark
*
*/

#ifndef __UPNP_ACTION_H__
#define __UPNP_ACTION_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "PropertyList.h"

TINY_BEGIN_DECLS

struct _UpnpAction;
typedef struct _UpnpAction UpnpAction;

UPNP_API UpnpAction * UpnpAction_New(void);
UPNP_API void UpnpAction_Delete(UpnpAction *thiz);

UPNP_API void UpnpAction_SetParentService(UpnpAction *thiz, void *service);
UPNP_API void * UpnpAction_GetParentService(UpnpAction *thiz);

UPNP_API PropertyList* UpnpAction_GetArgumentList(UpnpAction *thiz);
UPNP_API PropertyList* UpnpAction_GetResultList(UpnpAction *thiz);

UPNP_API PropertyList* UpnpAction_GetPropertyList(UpnpAction *thiz);
UPNP_API TinyRet UpnpAction_SetPropertyValue(UpnpAction *thiz, const char *propertyName, const char *value);
UPNP_API const char * UpnpAction_GetPropertyValue(UpnpAction *thiz, const char *propertyName);


TINY_END_DECLS

#endif /* __UPNP_ACTION_H__ */
