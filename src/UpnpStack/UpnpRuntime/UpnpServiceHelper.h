/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpServiceHelper.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_SERVICE_HELPER_H__
#define __UPNP_SERVICE_HELPER_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpService.h"

TINY_BEGIN_DECLS


UPNP_API TinyRet UpnpServiceHelper_UpdateScpdUrl(UpnpService *service);
UPNP_API TinyRet UpnpServiceHelper_UpdateCtrlUrl(UpnpService *service);
UPNP_API TinyRet UpnpServiceHelper_UpdateEventUrl(UpnpService *service);
UPNP_API TinyRet UpnpServiceHelper_UpdateServiceId(UpnpService *service);


TINY_END_DECLS

#endif /* __UPNP_SERVICE_HELPER_H__ */
