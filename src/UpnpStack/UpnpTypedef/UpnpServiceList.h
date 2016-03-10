/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpServiceList.h
*
* @remark
*
*/

#ifndef __UPNP_SERVICE_LIST_H__
#define __UPNP_SERVICE_LIST_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpService.h"

TINY_BEGIN_DECLS


struct _UpnpServiceList;
typedef struct _UpnpServiceList UpnpServiceList;

UPNP_API UpnpServiceList * UpnpServiceList_New(void);
UPNP_API void UpnpServiceList_Delete(UpnpServiceList * thiz);

UPNP_API TinyRet UpnpServiceList_AddService(UpnpServiceList * thiz, UpnpService *service);

UPNP_API uint32_t UpnpServiceList_GetSize(UpnpServiceList *thiz);
UPNP_API UpnpService * UpnpServiceList_GetServiceAt(UpnpServiceList *thiz, uint32_t index);
UPNP_API UpnpService * UpnpServiceList_GetService(UpnpServiceList *thiz, const char *serviceId);


TINY_END_DECLS

#endif /* __UPNP_SERVICE_LIST_H__ */
