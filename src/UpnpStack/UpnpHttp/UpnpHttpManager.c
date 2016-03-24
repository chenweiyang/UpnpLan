/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpHttpManager.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpHttpManager.h"
#include "tiny_log.h"
#include "tiny_memory.h"

#define TAG         "UpnpHttpManager"

UpnpHttpManager * UpnpHttpManager_New(void)
{
    UpnpHttpManager *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpHttpManager *)tiny_malloc(sizeof(UpnpHttpManager));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpHttpManager_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpHttpManager_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpHttpManager_Construct(UpnpHttpManager *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpHttpManager));

        ret = UpnpHttpClient_Construct(&thiz->client);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpClient_Construct failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        ret = UpnpHttpServer_Construct(&thiz->server);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_Construct failed");
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

void UpnpHttpManager_Dispose(UpnpHttpManager *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHttpServer_Dispose(&thiz->server);
    UpnpHttpClient_Dispose(&thiz->client);
}

void UpnpHttpManager_Delete(UpnpHttpManager *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpHttpManager_Dispose(thiz);
    tiny_free(thiz);
}