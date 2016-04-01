/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpUpnpArgument.h
*
* @remark
*
*/

#ifndef __UpnpArgument_H__
#define __UpnpArgument_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpArgumentDirection.h"

TINY_BEGIN_DECLS


struct _UpnpArgument;
typedef struct _UpnpArgument UpnpArgument;

UPNP_API UpnpArgument * UpnpArgument_New(const char *name, UpnpArgumentDirection direction, const char *relatedStateVariable);
UPNP_API void UpnpArgument_Delete(UpnpArgument *thiz);

UPNP_API TinyRet UpnpArgument_SetName(UpnpArgument *thiz, const char *name);
UPNP_API TinyRet UpnpArgument_SetDirection(UpnpArgument *thiz, UpnpArgumentDirection direction);
UPNP_API TinyRet UpnpArgument_SetRelatedStateVariable(UpnpArgument *thiz, const char *relatedStateVariable);

UPNP_API const char * UpnpArgument_GetName(UpnpArgument *thiz);
UPNP_API UpnpArgumentDirection UpnpArgument_GetDirection(UpnpArgument *thiz);
UPNP_API const char * UpnpArgument_GetRelatedStateVariable(UpnpArgument *thiz);


TINY_END_DECLS

#endif /* __UpnpArgument_H__ */