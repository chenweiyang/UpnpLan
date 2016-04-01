/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpDocumentGetter.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpDocumentGetter.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG     "UpnpDocumentGetter"

static void OnGet(UpnpHttpConnection *conn, const char *uri, void *ctx)
{
    UpnpDocumentGetter *thiz = (UpnpDocumentGetter *)ctx;

    LOG_D(TAG, "OnGet: %s", uri);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        char content[UPNP_DOCUMENT_LEN];
        uint32_t contentLength = 0;

        memset(content, 0, UPNP_DOCUMENT_LEN);
        contentLength = UpnpProvider_GetDocument(thiz->provider, uri, content, UPNP_DOCUMENT_LEN);
        if (contentLength == 0)
        {
            UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
            break;
        }

        UpnpHttpConnection_SendFileContent(conn, content, contentLength);
    } while (0);

    UpnpProvider_Unlock(thiz->provider);
}

UpnpDocumentGetter * UpnpDocumentGetter_New(UpnpHttpManager *http, UpnpProvider *provider)
{
    UpnpDocumentGetter *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpDocumentGetter *)tiny_malloc(sizeof(UpnpDocumentGetter));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpDocumentGetter_Construct(thiz, http, provider);
        if (RET_FAILED(ret))
        {
            UpnpDocumentGetter_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpDocumentGetter_Construct(UpnpDocumentGetter *thiz, UpnpHttpManager *http, UpnpProvider *provider)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpDocumentGetter));
        thiz->http = http;
        thiz->provider = provider;

        ret = UpnpHttpServer_RegisterGetHandler(&http->server, OnGet, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterGetHandler: failed");
            break;
        }
    } while (0);

    return ret;
}

void UpnpDocumentGetter_Dispose(UpnpDocumentGetter *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (RET_FAILED(UpnpHttpServer_UnregisterGetHandler(&thiz->http->server)))
    {
        LOG_E(TAG, "UpnpHttpServer_UnregisterGetHandler: failed");
    }

    thiz->http = NULL;
    thiz->provider = NULL;
}

void UpnpDocumentGetter_Delete(UpnpDocumentGetter *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpDocumentGetter_Dispose(thiz);
    tiny_free(thiz);
}