/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpActionInvoker.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpActionInvoker.h"
#include "tiny_memory.h"

UpnpActionInvoker * UpnpActionInvoker_New(UpnpHttpManager *http)
{
    UpnpActionInvoker *thiz = NULL;

    do
    {
        thiz = (UpnpActionInvoker *)tiny_malloc(sizeof(UpnpActionInvoker));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpActionInvoker_Construct(thiz, http)))
        {
            UpnpActionInvoker_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpActionInvoker_Construct(UpnpActionInvoker *thiz, UpnpHttpManager *http)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpActionInvoker));
        thiz->http = http;
    } while (0);

    return ret;
}

void UpnpActionInvoker_Dispose(UpnpActionInvoker *thiz)
{
    RETURN_IF_FAIL(thiz);
    
    thiz->http = NULL;
}

void UpnpActionInvoker_Delete(UpnpActionInvoker *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpActionInvoker_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpActionInvoker_Invoke(UpnpActionInvoker *thiz, UpnpAction *action, UpnpError *error)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(error, TINY_RET_E_ARG_NULL);

    return UpnpHttpClient_Post(&thiz->http->client, action, error, UPNP_TIMEOUT);
}