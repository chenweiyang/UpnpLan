/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpState.h
*
* @remark
*
*/

#ifndef __STATE_H__
#define __STATE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "upnp_define.h"
#include "PropertyDefinition.h"
#include "PropertyValue.h"

TINY_BEGIN_DECLS


typedef struct _UpnpState
{
    void *service;
    bool sendEvents;
    PropertyDefinition definition;
    PropertyValue value;
} UpnpState;

UPNP_API UpnpState * UpnpState_New();
UPNP_API void UpnpState_Delete(UpnpState *thiz);

UPNP_API TinyRet UpnpState_Construct(UpnpState *thiz);
UPNP_API void UpnpState_Dispose(UpnpState *thiz);

UPNP_API void UpnpState_Copy(UpnpState *dst, UpnpState *src);
UPNP_API TinyRet UpnpState_Initialize(UpnpState *thiz, const char *name, ObjectType *dataType, Object *data, bool sendEvents, void *service);


TINY_END_DECLS

#endif /* __STATE_H__ */
