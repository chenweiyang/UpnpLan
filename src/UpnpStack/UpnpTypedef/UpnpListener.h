/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpListener.h
*
* @remark
*      set tabstop4
*      set shiftwidth4
*      set expandtab
*/

#ifndef __UPNP_LISTENER_H__
#define __UPNP_LISTENER_H__

#include "tiny_base.h"
#include "UpnpDeviceSummary.h"
#include "UpnpUri.h"
#include "UpnpEvent.h"

TINY_BEGIN_DECLS


typedef void(*UpnpDeviceListener)(UpnpDeviceSummary *deviceSummary, bool alive, void *ctx);
typedef bool(*UpnpDeviceFilter)(UpnpUri *uri, void *ctx);
typedef void(*UpnpEventListener)(UpnpEvent *event, void *ctx);
typedef uint32_t(*UpnpActionHandler)(UpnpAction *action, void *ctx);


TINY_END_DECLS

#endif /* __UPNP_LISTENER_H__ */
