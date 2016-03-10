/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpActionList.h
*
* @remark
*
*/

#ifndef __UPNP_ACTION_LIST_H__
#define __UPNP_ACTION_LIST_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpAction.h"

TINY_BEGIN_DECLS


struct _ActionList;
typedef struct _ActionList UpnpActionList;

UPNP_API UpnpActionList * UpnpActionList_New(void);
UPNP_API void UpnpActionList_Delete(UpnpActionList * thiz);

UPNP_API TinyRet UpnpActionList_AddAction(UpnpActionList * thiz, UpnpAction *action);

UPNP_API uint32_t UpnpActionList_GetSize(UpnpActionList *thiz);
UPNP_API UpnpAction * UpnpActionList_GetActionAt(UpnpActionList *thiz, uint32_t index);
UPNP_API UpnpAction * UpnpActionList_GetAction(UpnpActionList *thiz, const char *actionName);


TINY_END_DECLS

#endif /* __UPNP_ACTION_LIST_H__ */
