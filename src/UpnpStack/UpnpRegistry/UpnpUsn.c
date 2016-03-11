/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpUsn.C
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpUsn.h"
#include "tiny_memory.h"
#include "tiny_str_equal.h"
#include "tiny_str_split.h"

UpnpUsn * UpnpUsn_New(void)
{
    UpnpUsn *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpUsn *)tiny_malloc(sizeof(UpnpUsn));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpUsn_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpUsn_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet UpnpUsn_Construct(UpnpUsn *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpUsn));

        ret = TinyUuid_Construct(&thiz->uuid);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUri_Construct(&thiz->uri);
        if (RET_FAILED(ret))
        {
            break;
        }
    }
    while (0);

    return ret;
}

TinyRet UpnpUsn_Dispose(UpnpUsn *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpUri_Dispose(&thiz->uri);
    TinyUuid_Dispose(&thiz->uuid);

    return TINY_RET_OK;
}

void UpnpUsn_Delete(UpnpUsn *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpUsn_Dispose(thiz);
    tiny_free(thiz);
}

void UpnpUsn_Copy(UpnpUsn *dst, UpnpUsn *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    strncpy(dst->string, src->string, UPNP_USN_LEN);
    TinyUuid_Copy(&dst->uuid, &src->uuid);
    strncpy(dst->uuid_string, src->uuid_string, UPNP_UUID_LEN); 
    UpnpUri_Copy(&dst->uri, &src->uri);
}

TinyRet UpnpUsn_Initialize_Uuid(UpnpUsn *thiz, const char *uuid)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(uuid, TINY_RET_E_ARG_NULL);

    do
    {
        ret = TinyUuid_ParseFromString(&thiz->uuid, uuid);
        if (RET_FAILED(ret))
        {
            break;
        }

        tiny_snprintf(thiz->string, UPNP_USN_LEN, "uuid:%s", uuid);
    }
    while (0);

    return ret;
}

TinyRet UpnpUsn_Initialize_Uuid_UpnpUri(UpnpUsn *thiz, const char *uuid, UpnpUri *uri)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(uuid, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(uri, TINY_RET_E_ARG_NULL);

    do
    {
        ret = TinyUuid_ParseFromString(&thiz->uuid, uuid);
        if (RET_FAILED(ret))
        {
            break;
        }

        UpnpUri_Copy(&thiz->uri, uri);
        tiny_snprintf(thiz->string, UPNP_USN_LEN, "uuid:%s::%s", uuid, uri->string);
    }
    while (0);

    return ret;
}

TinyRet UpnpUsn_Parse(UpnpUsn *thiz, const char *string, bool strict_uuid)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(string, TINY_RET_E_ARG_NULL);

    do
    {
        char group[2][128];
        uint32_t count = 0;
        const char *p = NULL;

        memset(group, 0, 2 * 128);
        count = str_split(string, "::", group, 2);
        if (count == 0)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        // uuid
        p = strstr(group[0], "uuid:");
        if (p == NULL)
        {
            p = strstr(group[0], "UUID:");
        }

        if (p == NULL)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        p += 5;

        if (strict_uuid)
        {
            ret = TinyUuid_ParseFromString(&thiz->uuid, p);
            if (RET_FAILED(ret))
            {
                break;
            }
        }
        else
        {
            strncpy(thiz->uuid_string, p, UPNP_UUID_LEN);
        }

        // uri
        if (count == 2)
        {
            ret = UpnpUri_Parse(&thiz->uri, group[1], strict_uuid);
            if (RET_FAILED(ret))
            {
                break;
            }
        }
    }
    while (0);

    if (RET_SUCCEEDED(ret))
    {
        strncpy(thiz->string, string, UPNP_USN_LEN);
    }

    return ret;
}
