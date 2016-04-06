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
#include "tiny_log.h"

#define TAG     "UpnpHost"

UpnpHost * UpnpHost_New(UpnpHttpManager *http, UpnpProvider *provider)
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

        ret = UpnpHost_Construct(thiz, http, provider);
        if (RET_FAILED(ret))
        {
            UpnpHost_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpHost_Construct(UpnpHost *thiz, UpnpHttpManager *http, UpnpProvider *provider)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpHost));

        ret = UpnpActionExecutor_Construct(&thiz->actionExecutor, http, provider);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpActionExecutor_Construct: failed");
            break;
        }

        ret = UpnpDocumentGetter_Construct(&thiz->documentGetter, http, provider);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpDocumentGetter_Construct: failed");
            break;
        }

        ret = UpnpGenaServer_Construct(&thiz->genaServer, http, provider);
    } while (0);

    return ret;
}

void UpnpHost_Dispose(UpnpHost *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpGenaServer_Dispose(&thiz->genaServer);
    UpnpActionExecutor_Dispose(&thiz->actionExecutor);
    UpnpDocumentGetter_Dispose(&thiz->documentGetter);
}

void UpnpHost_Delete(UpnpHost *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHost_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpHost_Start(UpnpHost *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpGenaServer_Start(&thiz->genaServer);
}

TinyRet UpnpHost_Stop(UpnpHost *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return UpnpGenaServer_Stop(&thiz->genaServer);
}