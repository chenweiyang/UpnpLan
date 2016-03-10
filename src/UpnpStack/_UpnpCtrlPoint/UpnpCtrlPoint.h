/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpCtrlPoint.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_CTRL_POINT_H__
#define __UPNP_CTRL_POINT_H__

#include "ct_common.h"
#include "upnp_api.h"
#include "UpnpDeviceSummary.h"
#include "UpnpDevice.h"
#include "UpnpAction.h"
#include "UpnpSubscription.h"

CT_BEGIN_DECLS


UPNP_API void UpnpStackInit(void);

struct _UpnpCtrlPoint;
typedef struct _UpnpCtrlPoint UpnpCtrlPoint;

UPNP_API UpnpCtrlPoint * UpnpCtrlPoint_New(void);
UPNP_API void UpnpCtrlPoint_Delete(UpnpCtrlPoint *thiz);
typedef void(*UpnpDeviceListener)(UpnpDeviceSummary *deviceSummary, bool alive, void *ctx);
typedef bool(*UpnpDeviceFilter)(const char *schemas, const char *deviceType, void *ctx);
UPNP_API CtRet UpnpCtrlPoint_SetListener(UpnpCtrlPoint *thiz, UpnpDeviceListener listener, UpnpDeviceFilter filter, void *ctx);
UPNP_API CtRet UpnpCtrlPoint_Start(UpnpCtrlPoint *thiz);
UPNP_API CtRet UpnpCtrlPoint_Stop(UpnpCtrlPoint *thiz);
UPNP_API CtRet UpnpCtrlPoint_Search(UpnpCtrlPoint *thiz);
UPNP_API UpnpDevice * UpnpCtrlPoint_LoadDeviceDescription(UpnpDeviceSummary *summary);
UPNP_API CtRet UpnpCtrlPoint_Invoke(UpnpCtrlPoint *thiz, UpnpAction *action, UpnpError *error);
UPNP_API CtRet UpnpCtrlPoint_Subscribe(UpnpCtrlPoint *thiz, UpnpSubscription *subscription, UpnpError *error);
UPNP_API CtRet UpnpCtrlPoint_Unsubscribe(UpnpCtrlPoint *thiz, UpnpError *error);


CT_END_DECLS

#endif /* __UPNP_CTRL_POINT_H__ */