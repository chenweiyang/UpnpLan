/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpObjectFactory.c
*
* @remark
*
*/

#include "UpnpObjectFactory.h"
#include "tiny_log.h"

#define TAG         "UpnpObjectFactory"

UpnpObject * UpnpObjectFactory_CreateByAlive(SsdpAlive *alive, const char *ip, UpnpValidator *validator)
{
    UpnpObject *obj = NULL;

    do
    {
        if (! UpnpValidator_ValidateNT(validator, alive->nt))
        {
            break;
        }

        obj = UpnpObject_New();
        if (obj == NULL)
        {
            LOG_E(TAG, "UpnpObject_New failed");
            break;
        }

        UpnpObject_SetNt(obj, alive->nt, validator->strictedUuid);
        UpnpObject_SetCacheControl(obj, alive->cache_control);
        UpnpObject_SetUsn(obj, alive->usn);
        UpnpObject_SetIp(obj, ip);
        UpnpObject_SetLocation(obj, alive->location);
        UpnpObject_SetStackInfo(obj, alive->server);
        UpnpObject_UpdateNextNotify(obj);
    } while (0);

    return obj;
}

UpnpObject * UpnpObjectFactory_CreateByResponse(SsdpResponse *response, const char *ip, UpnpValidator *validator)
{
    UpnpObject *obj = NULL;

    do
    {
        if (!UpnpValidator_ValidateNT(validator, response->st))
        {
            break;
        }

        obj = UpnpObject_New();
        if (obj == NULL)
        {
            LOG_E(TAG, "UpnpObject_New failed");
            break;
        }

        UpnpObject_SetNt(obj, response->st, validator->strictedUuid);
        UpnpObject_SetCacheControl(obj, response->cache_control);
        UpnpObject_SetUsn(obj, response->usn);
        UpnpObject_SetIp(obj, ip);
        UpnpObject_SetLocation(obj, response->location);
        UpnpObject_SetStackInfo(obj, response->server);
        UpnpObject_UpdateNextNotify(obj);
    } while (0);

    return obj;
}