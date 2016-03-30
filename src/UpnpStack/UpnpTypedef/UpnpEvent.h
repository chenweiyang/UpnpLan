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

TINY_BEGIN_DECLS


struct _UpnpEvent;
typedef struct _UpnpEvent UpnpEvent;

UPNP_API UpnpEvent * UpnpEvent_New(void);
UPNP_API TinyRet UpnpEvent_Construct(UpnpEvent *thiz);
UPNP_API void UpnpEvent_Dispose(UpnpEvent *thiz);
UPNP_API void UpnpEvent_Delete(UpnpEvent *thiz);

UPNP_API TinyRet UpnpEvent_SetConnection(UpnpEvent *thiz, const char *connection);
UPNP_API TinyRet UpnpEvent_SetNt(UpnpEvent *thiz, const char *nt);
UPNP_API TinyRet UpnpEvent_SetNts(UpnpEvent *thiz, const char *nts);
UPNP_API TinyRet UpnpEvent_SetSid(UpnpEvent *thiz, const char *sid);
UPNP_API TinyRet UpnpEvent_SetSeq(UpnpEvent *thiz, const char *seq);
UPNP_API const char * UpnpEvent_GetConnection(UpnpEvent *thiz);
UPNP_API const char * UpnpEvent_GetNt(UpnpEvent *thiz);
UPNP_API const char * UpnpEvent_GetNts(UpnpEvent *thiz);
UPNP_API const char * UpnpEvent_GetSid(UpnpEvent *thiz);
UPNP_API const char * UpnpEvent_GetSeq(UpnpEvent *thiz);

UPNP_API TinyRet UpnpEvent_SetArgumentValue(UpnpEvent *thiz, const char *argumentName, const char *value);
UPNP_API const char * UpnpEvent_GetArgumentValue(UpnpEvent *thiz, const char *argumentName);

UPNP_API TinyRet UpnpEvent_Parse(UpnpEvent *thiz,
    const char *nt,
    const char *nts,
    const char *sid,
    const char *seq,
    const char *content,
    uint32_t contentLength);

UPNP_API TinyRet UpnpEvent_ToString(UpnpEvent *thiz, char *bytes, uint32_t len);


TINY_END_DECLS

#endif /* __UPNP_EVENT_H__ */
