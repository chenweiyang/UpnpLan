/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObject.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_OBJECT_H__
#define __UPNP_OBJECT_H__

#include "tiny_base.h"
#include "upnp_define.h"
#include "upnp_api.h"
#include "UpnpUri.h"

TINY_BEGIN_DECLS


struct _UpnpObject;
typedef struct _UpnpObject UpnpObject;

UPNP_API UpnpObject * UpnpObject_New(void);
UPNP_API TinyRet UpnpObject_Construct(UpnpObject *thiz);
UPNP_API TinyRet UpnpObject_Dispose(UpnpObject *thiz);
UPNP_API void UpnpObject_Delete(UpnpObject *thiz);
UPNP_API void UpnpObject_Copy(UpnpObject *dst, UpnpObject *src);

UPNP_API TinyRet UpnpObject_SetNt(UpnpObject *thiz, const char *nt, bool strict_uuid);
UPNP_API void UpnpObject_SetIp(UpnpObject *thiz, const char *ip);
UPNP_API void UpnpObject_SetLocation(UpnpObject *thiz, const char *location);
UPNP_API void UpnpObject_SetStackInfo(UpnpObject *thiz, const char *info);
UPNP_API void UpnpObject_SetUsn(UpnpObject *thiz, const char *usn);
UPNP_API void UpnpObject_SetCacheControl(UpnpObject *thiz, const char *cache_control);
UPNP_API void UpnpObject_SetAge(UpnpObject *thiz, uint64_t age);
UPNP_API void UpnpObject_UpdateNextNotify(UpnpObject *thiz);

UPNP_API UpnpUri * UpnpObject_GetNt(UpnpObject *thiz);
UPNP_API const char * UpnpObject_GetIp(UpnpObject *thiz);
UPNP_API const char * UpnpObject_GetLocation(UpnpObject *thiz);
UPNP_API const char * UpnpObject_GetStackInfo(UpnpObject *thiz);
UPNP_API const char * UpnpObject_GetUsn(UpnpObject *thiz);
UPNP_API uint64_t UpnpObject_GetAge(UpnpObject *thiz);
UPNP_API uint64_t UpnpObject_GetNextNotify(UpnpObject *thiz);


TINY_END_DECLS

#endif /* __UPNP_OBJECT_H__ */