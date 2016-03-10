/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpMonitor.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_MONITOR_H__
#define __UPNP_MONITOR_H__

#include "ct_common.h"
#include "upnp_api.h"
#include "UpnpSubscription.h"

CT_BEGIN_DECLS


struct _UpnpMonitor;
typedef struct _UpnpMonitor UpnpMonitor;

UPNP_API UpnpMonitor * UpnpMonitor_New(void);
UPNP_API void UpnpMonitor_Delete(UpnpMonitor *thiz);

UPNP_API CtRet UpnpMonitor_Subscribe(UpnpMonitor *thiz, UpnpSubscription *subscription, UpnpError *error);
UPNP_API CtRet UpnpMonitor_Unsubscribe(UpnpMonitor *thiz, UpnpError *error);


CT_END_DECLS

#endif /* __UPNP_MONITOR_H__ */
