/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHost.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpHost.h"
#include "tiny_memory.h"

#define TAG     "UpnpHost"

UpnpHost * UpnpHost_New(UpnpHttpManager *http)
{
    UpnpHost *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpHost *)tiny_malloc(sizeof(UpnpHost));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpHost_Construct(thiz, http);
        if (RET_FAILED(ret))
        {
            UpnpHost_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpHost_Construct(UpnpHost *thiz, UpnpHttpManager *http)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpHost));
        thiz->http = http;

        ret = TinyList_Construct(&thiz->list);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            break;
        }

    } while (0);

    return ret;
}

void UpnpHost_Dispose(UpnpHost *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHost_Stop(thiz);

    TinyMutex_Dispose(&thiz->mutex);
    TinyList_Dispose(&thiz->list);
}

void UpnpHost_Delete(UpnpHost *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHost_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpHost_Start(UpnpHost *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;
}

TinyRet UpnpHost_Stop(UpnpHost *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;
}

TinyRet UpnpHost_AddDevice(UpnpHost *thiz, UpnpDevice *device, UpnpActionHandler handler, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;
}

TinyRet UpnpHost_RemoveDevice(UpnpHost *thiz, UpnpDevice *device)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;
}

TinyRet UpnpHost_SendEvents(UpnpHost *thiz, UpnpService *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return ret;
}