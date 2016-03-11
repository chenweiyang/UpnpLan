/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObject.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpObject.h"
#include "UpnpUri.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "tiny_str_equal.h"
#include "tiny_str_get_value.h"


#define TAG                     "UpnpObject"
#define UPNP_USN_LEN            128

struct _UpnpObject
{
    UpnpUri nt;
    char ip[TINY_IP_LEN];
    char location[TINY_URL_LEN];
    char stack_info[UPNP_STACK_INFO_LEN];
    char usn[UPNP_USN_LEN];
    uint64_t max_age;
    uint64_t next_notify;
};

UpnpObject * UpnpObject_New(void)
{
    UpnpObject *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpObject *)tiny_malloc(sizeof(UpnpObject));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpObject_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpObject_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet UpnpObject_Construct(UpnpObject *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpObject));

        ret = UpnpUri_Construct(&thiz->nt);
        if (RET_FAILED(ret))
        {
            break;
        }
    }
    while (0);

    return ret;
}

TinyRet UpnpObject_Dispose(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpUri_Dispose(&thiz->nt);

    return TINY_RET_OK;
}

void UpnpObject_Delete(UpnpObject *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpObject_Dispose(thiz);
    tiny_free(thiz);
}

void UpnpObject_Copy(UpnpObject *dst, UpnpObject *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    UpnpUri_Copy(&dst->nt, &src->nt);
    strncpy(dst->ip, src->ip, TINY_IP_LEN);
    strncpy(dst->location, src->location, TINY_URL_LEN);
    strncpy(dst->stack_info, src->stack_info, UPNP_STACK_INFO_LEN);
    strncpy(dst->usn, src->usn, UPNP_USN_LEN);
    dst->max_age = src->max_age;
    dst->next_notify = src->next_notify;
}

TinyRet UpnpObject_SetNt(UpnpObject *thiz, const char *nt, bool strict_uuid)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(nt, TINY_RET_E_ARG_NULL);

    ret = UpnpUri_Parse(&thiz->nt, nt, strict_uuid);
    if (RET_FAILED(ret))
    {
        LOG_D(TAG, "UpnpUri_Parse failed: %s", TINY_RET_to_str(ret));
    }

    return ret;
}

void UpnpObject_SetIp(UpnpObject *thiz, const char *ip)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(ip);
    strncpy(thiz->ip, ip, TINY_IP_LEN);
}

void UpnpObject_SetLocation(UpnpObject *thiz, const char *location)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(location);
    strncpy(thiz->location, location, TINY_URL_LEN);
}

void UpnpObject_SetStackInfo(UpnpObject *thiz, const char *info)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(info);
    strncpy(thiz->stack_info, info, UPNP_STACK_INFO_LEN);
}

void UpnpObject_SetUsn(UpnpObject *thiz, const char *usn)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(usn);
    strncpy(thiz->usn, usn, UPNP_USN_LEN);
}

void UpnpObject_SetCacheControl(UpnpObject *thiz, const char *cache_control)
{
    char age[32];

    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(age);

    memset(age, 0, 32);
    str_get_value(cache_control, strlen(cache_control), "max-age=", NULL, age, 32);

    thiz->max_age = atoi(age);
}

void UpnpObject_SetAge(UpnpObject *thiz, uint64_t age)
{
    RETURN_IF_FAIL(thiz);
    thiz->max_age = age;
}

void UpnpObject_UpdateNextNotify(UpnpObject *thiz)
{
    RETURN_IF_FAIL(thiz);
    thiz->next_notify = (tiny_getusec() / 1000 / 1000) + thiz->max_age;
}

UpnpUri * UpnpObject_GetNt(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    return &thiz->nt;
}

const char * UpnpObject_GetIp(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    return thiz->ip;
}

const char * UpnpObject_GetLocation(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    return thiz->location;
}

const char * UpnpObject_GetStackInfo(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    return thiz->stack_info;
}

const char * UpnpObject_GetUsn(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    return thiz->usn;
}

uint64_t UpnpObject_GetAge(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);
    return thiz->max_age;
}

uint64_t UpnpObject_GetNextNotify(UpnpObject *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);
    return thiz->next_notify;
}