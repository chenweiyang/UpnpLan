/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpStateList.h
*
* @remark
*
*/

#ifndef __UPNP_STATE_LIST_H__
#define __UPNP_STATE_LIST_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpState.h"

TINY_BEGIN_DECLS


struct _UpnpStateList;
typedef struct _UpnpStateList UpnpStateList;

UPNP_API UpnpStateList * UpnpStateList_New(void);
UPNP_API void UpnpStateList_Delete(UpnpStateList * thiz);

UPNP_API TinyRet UpnpStateList_InitState(UpnpStateList *thiz, const char *name, ObjectType *type, bool sendEvents, void *service);
UPNP_API TinyRet UpnpStateList_SetStateValue(UpnpStateList *thiz, const char *name, Object *data);
UPNP_API Object * UpnpStateList_GetStateValue(UpnpStateList *thiz, const char *name);

UPNP_API uint32_t UpnpStateList_GetSize(UpnpStateList *thiz);
UPNP_API UpnpState * UpnpStateList_GetStateAt(UpnpStateList *thiz, uint32_t index);
UPNP_API UpnpState * UpnpStateList_GetState(UpnpStateList *thiz, const char *name);


TINY_END_DECLS

#endif /* __UPNP_STATE_LIST_H__ */
