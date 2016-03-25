/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   ServiceUnsubResponse.c
*
* @remark
*
*/

#include "ServiceUnsubResponse.h"
#include "tiny_log.h"

#define TAG             "ServiceUnsubResponse"

TinyRet ServiceUnsubFromResponse(UpnpSubscription *subscription, UpnpError *error, HttpMessage *response)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        error->code = HttpMessage_GetStatusCode(response);
        strncpy(error->description, HttpMessage_GetStatus(response), UPNP_ERR_DESCRIPTION_LEN);

        if (error->code != HTTP_STATUS_OK)
        {
            LOG_D(TAG, "Subscribe failed: %d %s", error->code, error->description);
            ret = TINY_RET_E_UPNP_INVOKE_FAILED;
            break;
        }
    } while (0);

    return ret;
}