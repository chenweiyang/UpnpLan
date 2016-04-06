/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpStateVariable.h
*
* @remark
*
*/

#ifndef __UPNP_STATE_VARIABLE_H__
#define __UPNP_STATE_VARIABLE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpStateVariableDefinition.h"
#include "DataValue.h"

TINY_BEGIN_DECLS


typedef struct _UpnpStateVariable
{
    void *service;
    bool sendEvents;
    bool isChanged;
    UpnpStateVariableDefinition definition;
    DataValue value;
} UpnpStateVariable;

UPNP_API UpnpStateVariable * UpnpStateVariable_New();
UPNP_API void UpnpStateVariable_Delete(UpnpStateVariable *thiz);

UPNP_API TinyRet UpnpStateVariable_Construct(UpnpStateVariable *thiz);
UPNP_API void UpnpStateVariable_Dispose(UpnpStateVariable *thiz);

UPNP_API void UpnpStateVariable_Copy(UpnpStateVariable *dst, UpnpStateVariable *src);
UPNP_API TinyRet UpnpStateVariable_Initialize(UpnpStateVariable *thiz, const char *name, const char *dataType, const char *defaultValue, const char *sendEvents);


TINY_END_DECLS

#endif /* __UPNP_STATE_VARIABLE_H__ */