/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpDeviceSummary.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_DEVICE_SUMMARY_H__
#define __UPNP_DEVICE_SUMMARY_H__

#include "ct_common.h"
#include "upnp_define.h"

CT_BEGIN_DECLS


typedef struct _UpnpDeviceSummary
{
    char deviceIp[CT_IP_LEN];
    char deviceId[UPNP_URI_LEN];
    char deviceUrl[CT_URL_LEN];
    char domainName[UPNP_DOMAIN_NAME_LEN];
    char deviceType[UPNP_TYPE_LEN];
    uint32_t deviceVersion;
    char upnpStackInfo[UPNP_STACK_INFO_LEN];
} UpnpDeviceSummary;


CT_END_DECLS

#endif /* __UPNP_DEVICE_SUMMARY_H__ */