/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSubscriber.c
*
* @remark
*
*/

#include "UpnpSubscriber.h"
#include "tiny_memory.h"

struct _UpnpSubscriber
{
    uint32_t timeout;
    char sid[UPNP_UUID_LEN];
    char callback[TINY_URL_LEN];
} ;

UpnpSubscriber * UpnpSubscriber_New()
{
    UpnpSubscriber *thiz = NULL;

    do
    {
        thiz = (UpnpSubscriber *)tiny_malloc(sizeof(UpnpSubscriber));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpSubscriber_Construct(thiz)))
        {
            UpnpSubscriber_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpSubscriber_Construct(UpnpSubscriber *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSubscriber));
        thiz->timeout = 0;
    } while (0);

    return ret;
}

void UpnpSubscriber_Dispose(UpnpSubscriber *thiz)
{
    RETURN_IF_FAIL(thiz);
}

void UpnpSubscriber_Delete(UpnpSubscriber *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpSubscriber_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpSubscriber_SetCallback(UpnpSubscriber *thiz, const char *callback)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    strncpy(thiz->callback, callback, TINY_URL_LEN);

    return TINY_RET_OK;
}

const char * UpnpSubscriber_GetCallback(UpnpSubscriber *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->callback;
}

TinyRet UpnpSubscriber_SetTimeout(UpnpSubscriber *thiz, uint32_t timeout)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->timeout = timeout;

    return TINY_RET_OK;
}

uint32_t UpnpSubscriber_GetTimeout(UpnpSubscriber *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->timeout;
}

TinyRet UpnpSubscriber_SetSid(UpnpSubscriber *thiz, const char *sid)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    strncpy(thiz->sid, sid, TINY_URL_LEN);

    return TINY_RET_OK;
}

const char * UpnpSubscriber_GetSid(UpnpSubscriber *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->sid;
}