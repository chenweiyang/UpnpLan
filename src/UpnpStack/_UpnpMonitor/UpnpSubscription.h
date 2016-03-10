/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSubscription.h
*
* @remark
*
*/

#ifndef __UPNP_SUBSCRIPTION_H__
#define __UPNP_SUBSCRIPTION_H__

#include "ct_common.h"
#include "UpnpEvent.h"
#include "UpnpService.h"

CT_BEGIN_DECLS


typedef void(*UpnpEventListener)(UpnpEvent *event, void *ctx);

typedef struct _UpnpSubscription
{
    UpnpService *service;
    UpnpEventListener listener;
    void *ctx;
    char subscribeId[UPNP_UUID_LEN];
    uint32_t timeout;
} UpnpSubscription;


CT_END_DECLS

#endif /* __UPNP_SUBSCRIPTION_H__ */
