/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpStateVariableDefinition.h
*
* @remark
*
*/

#ifndef __UPNP_STATE_VARIABLE_DEFINITION_H__
#define __UPNP_STATE_VARIABLE_DEFINITION_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "DataType.h"

TINY_BEGIN_DECLS


#define STATE_VARIABLE_NAME_LEN     128

typedef struct _UpnpStateVariableDefinition
{
    char name[STATE_VARIABLE_NAME_LEN];
    DataType dataType;

#if 0
    AllowedValueList allowedValueList;
    AllowedValueRange allowedValueRange;
#endif
} UpnpStateVariableDefinition;

UPNP_API void UpnpStateVariableDefinition_Construct(UpnpStateVariableDefinition *thiz);
UPNP_API void UpnpStateVariableDefinition_Dispose(UpnpStateVariableDefinition *thiz);
UPNP_API void UpnpStateVariableDefinition_Copy(UpnpStateVariableDefinition *dst, UpnpStateVariableDefinition *src);
UPNP_API void UpnpStateVariableDefinition_Initialize(UpnpStateVariableDefinition *thiz, const char *name, DataType *type);


TINY_END_DECLS

#endif /* __UPNP_STATE_VARIABLE_DEFINITION_H__ */
