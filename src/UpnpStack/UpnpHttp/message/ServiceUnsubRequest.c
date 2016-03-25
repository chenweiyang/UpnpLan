/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   ServiceUnsubRequest.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "ServiceUnsubRequest.h"
#include "UpnpDevice.h"
#include "UpnpDeviceDefinition.h"
#include "UpnpServiceDefinition.h"

TinyRet ServiceUnsubToRequest(UpnpSubscription *subscription, HttpMessage *request)
{
    UpnpDevice *device = NULL;
    const char *urlbase = NULL;
    const char *eventSubUrl = NULL;
    char sub_url[TINY_URL_LEN];

    RETURN_VAL_IF_FAIL(subscription, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, TINY_RET_E_ARG_NULL);

    device = (UpnpDevice *)UpnpService_GetParentDevice(subscription->service);
    urlbase = UpnpDevice_GetPropertyValue(device, UPNP_DEVICE_URLBase);
    eventSubUrl = UpnpService_GetPropertyValue(subscription->service, UPNP_SERVICE_EventSubURL);

    memset(sub_url, 0, TINY_URL_LEN);
    tiny_snprintf(sub_url, TINY_URL_LEN, "%s%s", urlbase, eventSubUrl);

    HttpMessage_SetRequest(request, "UNSUBSCRIBE", sub_url);
    HttpMessage_SetHeader(request, "User-Agent", UPNP_STACK_INFO);
    HttpMessage_SetHeader(request, "SID", subscription->subscribeId);

    return TINY_RET_OK;
}