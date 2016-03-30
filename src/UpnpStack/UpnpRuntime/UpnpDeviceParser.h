/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpDeviceParser.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_DEVICE_PARSER_H__
#define __UPNP_DEVICE_PARSER_H__

#include "tiny_base.h"
#include "UpnpDevice.h"

TINY_BEGIN_DECLS


TinyRet UpnpDeviceParser_Parse(const char *url, UpnpDevice *device, uint32_t timeout);
uint32_t UpnpDeviceParser_ToXml(UpnpDevice *device, char *xml, uint32_t len);


TINY_END_DECLS

#endif /* __UPNP_DEVICE_PARSER_H__ */
