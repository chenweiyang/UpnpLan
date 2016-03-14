/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpDeviceFactory.h
*
* @remark
*
*/

#ifndef __UPNP_DEVICE_FACTORY_H__
#define __UPNP_DEVICE_FACTORY_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpDevice.h"
#include "UpnpDeviceSummary.h"

TINY_BEGIN_DECLS


UPNP_API UpnpDevice * UpnpDeviceFactory_Create(UpnpDeviceSummary *summary);


TINY_END_DECLS

#endif /* __UPNP_DEVICE_FACTORY_H__ */