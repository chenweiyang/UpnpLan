/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpActionExecutor.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpActionExecutor.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "tiny_str_split.h"
#include "message/soap/SoapMessage.h"

#define TAG     "UpnpActionExecutor"

static void OnPost(UpnpHttpConnection *conn, const char *uri, const char *soapAction, const char *content, uint32_t contentLength, void *ctx)
{
    UpnpActionExecutor *thiz = (UpnpActionExecutor *)ctx;

    LOG_D(TAG, "OnPost: %s", uri);

    UpnpProvider_Lock(thiz->provider);
    do
    {
        uint32_t count = 0;
        char group[4][128];
        const char *serviceType = NULL;
        const char *actionName = NULL;

        memset(group, 0, 4 * 128);

        count = str_split(soapAction, "#", group, 4);
        if (count != 2)
        {
            UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
            break;
        }

        serviceType = group[0];
        actionName = group[1];

        UpnpAction *action = UpnpProvider_GetAction(thiz->provider, uri, serviceType, actionName);
        if (action == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
            break;
        }

        do
        {
            // Parse SoapMessage
            TinyRet ret = TINY_RET_OK;
            SoapMessage * soap = SoapMessage_New();

            ret = SoapMessage_Parse(soap, content, contentLength);
            if (RET_FAILED(ret))
            {
                UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
                break;
            }

            // SetArgument for Action
        } while (0);

    } while (0);
    UpnpProvider_Unlock(thiz->provider);
}

UpnpActionExecutor * UpnpActionExecutor_New(UpnpHttpManager *http, UpnpProvider *provider)
{
    UpnpActionExecutor *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpActionExecutor *)tiny_malloc(sizeof(UpnpActionExecutor));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpActionExecutor_Construct(thiz, http, provider);
        if (RET_FAILED(ret))
        {
            UpnpActionExecutor_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpActionExecutor_Construct(UpnpActionExecutor *thiz, UpnpHttpManager *http, UpnpProvider *provider)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpActionExecutor));
        thiz->http = http;
        thiz->provider = provider;

        ret = UpnpHttpServer_RegisterPostHandler(&http->server, OnPost, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterPostHandler: failed");
            break;
        }
    } while (0);

    return ret;
}

void UpnpActionExecutor_Dispose(UpnpActionExecutor *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (RET_FAILED(UpnpHttpServer_UnregisterPostHandler(&thiz->http->server)))
    {
        LOG_E(TAG, "UpnpHttpServer_UnregisterPostHandler: failed");
    }

    thiz->http = NULL;
    thiz->provider = NULL;
}

void UpnpActionExecutor_Delete(UpnpActionExecutor *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpActionExecutor_Dispose(thiz);
    tiny_free(thiz);
}