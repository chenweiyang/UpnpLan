/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   EventRequest.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "EventRequest.h"

TinyRet UpnpEventToRequest(UpnpEvent *event, HttpMessage *request)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        char data[1024 * 4];
        uint32_t size = 0;

        memset(data, 0, 1024 * 4);
        size = UpnpEvent_ToString(event, data, 1024 * 4);
        if (size == 0)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        HttpMessage_SetRequest(request, "NOTIFY", UpnpEvent_GetCallback(event));
        HttpMessage_SetHeader(request, "Content-Type", "text/xml;charset=\"utf-8\"");
        HttpMessage_SetHeader(request, "nt", UpnpEvent_GetNt(event));
        HttpMessage_SetHeader(request, "nts", UpnpEvent_GetNts(event));
        HttpMessage_SetHeader(request, "sid", UpnpEvent_GetSid(event));
        HttpMessage_SetHeader(request, "seq", UpnpEvent_GetSeq(event));
        HttpMessage_SetHeaderInteger(request, "Content-Length", size);
        HttpMessage_SetContentSize(request, size);
        HttpMessage_AddContentObject(request, data, size);
    } while (0);

    return ret;
}