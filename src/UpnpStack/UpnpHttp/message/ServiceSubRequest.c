/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   ServiceSubRequest.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "ServiceSubRequest.h"
#include "UpnpDevice.h"

TinyRet ServiceSubToRequest(UpnpSubscription *subscription, HttpMessage *request)
{
    UpnpDevice *device = NULL;
    const char *urlbase = NULL;
    const char *eventSubUrl = NULL;
    char sub_url[TINY_URL_LEN];

    RETURN_VAL_IF_FAIL(subscription, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(request, TINY_RET_E_ARG_NULL);

    device = (UpnpDevice *)UpnpService_GetParentDevice(subscription->service);
    urlbase = UpnpDevice_GetURLBase(device);
    eventSubUrl = UpnpService_GetEventSubURL(subscription->service);

    memset(sub_url, 0, TINY_URL_LEN);
    tiny_snprintf(sub_url, TINY_URL_LEN, "%s%s", urlbase, eventSubUrl);

    HttpMessage_SetRequest(request, "SUBSCRIBE", sub_url);
    HttpMessage_SetHeader(request, "User-Agent", UPNP_STACK_INFO);
    HttpMessage_SetHeader(request, "CALLBACK", UpnpSubscription_GetFullCallBack(subscription));
    HttpMessage_SetHeader(request, "NT", "upnp:event");
    HttpMessage_SetHeader(request, "TIMEOUT", "Second-infinite");

    return TINY_RET_OK;
}