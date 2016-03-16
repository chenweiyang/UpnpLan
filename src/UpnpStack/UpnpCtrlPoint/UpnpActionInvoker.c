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
#include "SoapClient.h"
#include "UpnpSoapHelper.h"
#include "tiny_memory.h"

static TinyRet UpnpActionInvoker_Construct(UpnpActionInvoker *thiz);
static void UpnpActionInvoker_Dispose(UpnpActionInvoker *thiz);

struct _UpnpActionInvoker
{
    SoapClient            * client;
};

UpnpActionInvoker * UpnpActionInvoker_New(void)
{
    UpnpActionInvoker *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpActionInvoker *)tiny_malloc(sizeof(UpnpActionInvoker));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpActionInvoker_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpActionInvoker_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet UpnpActionInvoker_Construct(UpnpActionInvoker *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpActionInvoker));

        thiz->client = SoapClient_New();
        if (thiz->client == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static void UpnpActionInvoker_Dispose(UpnpActionInvoker *thiz)
{
    RETURN_IF_FAIL(thiz);

    if (thiz->client != NULL) 
    {
        SoapClient_Delete(thiz->client);
    }
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

    do
    {
        SoapMessage * request = SoapMessage_New();
        if (request == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        do
        {
            ret = ActionToSoapMessage(action, request);
            if (RET_FAILED(ret))
            {
                break;
            }

            SoapMessage *response = SoapMessage_New();
            if (response == NULL)
            {
                ret = TINY_RET_E_NEW;
                break;
            }

            ret = SoapClient_Invoke(thiz->client, request, response, UPNP_TIMEOUT);
            if (RET_SUCCEEDED(ret))
            {
                ret = SoapMessageToActionResult(response, action, error);
            }

            SoapMessage_Delete(response);
        } while (0);

        SoapMessage_Delete(request);
    } while (0);

    return ret;
}