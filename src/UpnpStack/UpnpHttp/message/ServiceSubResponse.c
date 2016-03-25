/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   ServiceSubResponse.c
*
* @remark
*
*/

#include "ServiceSubResponse.h"
#include "tiny_log.h"
#include "upnp_timeout_util.h"

#define TAG             "ServiceSubResponse"

TinyRet ServiceSubFromResponse(UpnpSubscription *subscription, UpnpError *error, HttpMessage *response)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        const char *timeout = NULL;
        const char *sid = NULL;

        error->code = HttpMessage_GetStatusCode(response);
        strncpy(error->description, HttpMessage_GetStatus(response), UPNP_ERR_DESCRIPTION_LEN);

        if (error->code != HTTP_STATUS_OK)
        {
            LOG_D(TAG, "Subscribe failed: %d %s", error->code, error->description);
            ret = TINY_RET_E_UPNP_SUBSCRIBE_FAILED;
            break;
        }

        timeout = HttpMessage_GetHeaderValue(response, "Timeout");
        if (timeout == NULL)
        {
            LOG_D(TAG, "NOT FOUND: Timeout");
            ret = TINY_RET_E_UPNP_SUBSCRIBE_FAILED;
            break;
        }

        if (RET_FAILED(upnp_timeout_get_second(timeout, &subscription->timeout)))
        {
            LOG_D(TAG, "INVALID: %s", timeout);
            ret = TINY_RET_E_UPNP_SUBSCRIBE_FAILED;
            break;
        }

        sid = HttpMessage_GetHeaderValue(response, "SID");
        if (sid == NULL)
        {
            LOG_D(TAG, "NOT FOUND: SID");
            ret = TINY_RET_E_UPNP_SUBSCRIBE_FAILED;
            break;
        }

        strncpy(subscription->subscribeId, sid, UPNP_UUID_LEN);
    } while (0);

    return ret;
}