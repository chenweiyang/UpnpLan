/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpValidator.c
*
* @remark
*
*/

#include "UpnpValidator.h"
#include "UpnpUsn.h"
#include "tiny_log.h"

#define TAG                 "UpnpValidator"

TinyRet UpnpValidator_Construct(UpnpValidator *thiz)
{
    memset(thiz, 0, sizeof(UpnpValidator));
    return TINY_RET_OK;
}

void UpnpValidator_Dispose(UpnpValidator *thiz)
{
}

bool UpnpValidator_ValidateUSN(UpnpValidator *thiz, const char *string)
{
    bool result = false;

    do
    {
        TinyRet ret = TINY_RET_OK;
        UpnpUsn usn;

        ret = UpnpUsn_Construct(&usn);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUsn_Parse(&usn, string, thiz->strictedUuid);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpUsn_Parse failed: %s", tiny_ret_to_str(ret));
            break;
        }

        result = (thiz->filter == NULL) ? true : thiz->filter(&usn, thiz->ctx);

        UpnpUsn_Dispose(&usn);
    } while (0);

    return result;
}

bool UpnpValidator_ValidateNT(UpnpValidator *thiz, const char *string)
{
    bool result = true;

    do
    {
        UpnpUri uri;

        if (RET_FAILED(UpnpUri_Construct(&uri)))
        {
            result = false;
            break;
        }

        if (RET_FAILED(UpnpUri_Parse(&uri, string, thiz->strictedUuid)))
        {
            LOG_D(TAG, "UpnpUri_Parse failed");
            result = false;
            break;
        }

        UpnpUri_Dispose(&uri);
    } while (0);

    return result;
}