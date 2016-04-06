/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpGenaServer.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpGenaServer.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "UpnpSubscriber.h"

#define TAG     "UpnpGenaServer"

static void OnNotifyJobDelete(TinyWorker *worker, void *job, void *ctx)
{
    UpnpEvent * e = (UpnpEvent *)job;
    UpnpEvent_Delete(e);
}

static bool DoNotify(TinyWorker *worker, void *job, void *ctx)
{
    UpnpGenaServer *thiz = (UpnpGenaServer *)ctx;
    UpnpEvent * event = (UpnpEvent *)job;

    if (UpnpEvent_GetArgumentCount(event) > 0)
    {
        UpnpHttpClient_Notify(&thiz->http->client, event);
    }

    UpnpEvent_Delete(event);

    return true;
}

static void OnSubscribe(UpnpHttpConnection *conn, const char *uri, const char *callback, const char *nt, uint32_t timeout, void *ctx)
{
    UpnpGenaServer *thiz = (UpnpGenaServer *)ctx;

    LOG_D(TAG, "OnSubscribe: %s", uri);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        /**
         * add subscriber
         */
        UpnpService *service = UpnpProvider_GetService(thiz->provider, uri);
        if (service == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
            break;
        }

        if (RET_FAILED(UpnpService_GetSubscriber(service, callback)))
        {
            UpnpHttpConnection_SendError(conn, 404, "ALREADY SUBSCRIBED");
            break;
        }

        UpnpSubscriber *subscriber = UpnpSubscriber_New();
        UpnpSubscriber_SetCallback(subscriber, callback);
        UpnpSubscriber_SetTimeout(subscriber, timeout);
        UpnpSubscriber_SetSid(subscriber, "uuid:xxxxx");
        UpnpService_AddSubscriber(service, subscriber);

        /**
         * send response
         */
        UpnpHttpConnection_SendSubscribeResponse(conn,
            UpnpSubscriber_GetSid(subscriber),
            UpnpSubscriber_GetTimeout(subscriber));

        /**
         * send NOTIFY
         */
        do
        {
            uint32_t i = 0;
            uint32_t count = 0;
            UpnpEvent *event = NULL;

            event = UpnpEvent_New();
            if (event == NULL)
            {
                LOG_E(TAG, "UpnpEvent_New failed");
                break;
            }

            UpnpEvent_SetCallback(event, callback);
            UpnpEvent_SetConnection(event, "Close");
            UpnpEvent_SetNt(event, "upnp:event");
            UpnpEvent_SetNts(event, "upnp:propchange");
            UpnpEvent_SetSeq(event, "0");
            UpnpEvent_SetSid(event, UpnpSubscriber_GetSid(subscriber));

            count = UpnpService_GetStateVariableCount(service);
            for (i = 0; i < count; ++i)
            {
                UpnpStateVariable *v = (UpnpStateVariable *)UpnpService_GetStateVariableAt(service, i);
                if (v->sendEvents)
                {
                    const char *value = NULL;
                    char buffer[128];

                    memset(buffer, 0, 128);
                    value = buffer;

                    if (v->value.internalType == INTERNAL_STRING)
                    {
                        value = v->value.internalValue.stringValue;
                    }
                    else
                    {
                        if (RET_FAILED(DataValue_GetValue(&v->value, buffer, 128)))
                        {
                            LOG_E(TAG, "value invalid: %s", v->definition.name);
                            break;
                        }
                    }

                    UpnpEvent_SetArgumentValue(event, v->definition.name, value);
                }
            }

            TinyWorker_PutJob(&thiz->notifyWorker, event);
        } while (0);

    } while (0);

    UpnpProvider_Unlock(thiz->provider);
}

static void OnUnsubscribe(UpnpHttpConnection *conn, const char *uri, const char *sid, void *ctx)
{
    UpnpGenaServer *thiz = (UpnpGenaServer *)ctx;

    LOG_D(TAG, "OnUnsubscribe: %s", uri);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        /**
         * remove subscriber
         */
        UpnpService *service = UpnpProvider_GetService(thiz->provider, uri);
        if (service == NULL)
        {
            UpnpHttpConnection_SendError(conn, 404, "NOT FOUND");
            break;
        }

        if (RET_FAILED(UpnpService_RemoveSubscriber(service, sid)))
        {
            UpnpHttpConnection_SendError(conn, 404, "NOT SUBSCRIBED");
            break;
        }

        /**
         * send response
         */
        UpnpHttpConnection_SendOk(conn);
    } while (0);

    UpnpProvider_Unlock(thiz->provider);
}

static void OnServiceChanged(UpnpService *service, void *ctx)
{
    UpnpGenaServer *thiz = (UpnpGenaServer *)ctx;

    LOG_D(TAG, "OnServiceChanged");

    do
    {
        uint32_t i = 0;
        uint32_t count = 0;
        UpnpEvent *event = NULL;

        event = UpnpEvent_New();
        if (event == NULL)
        {
            LOG_E(TAG, "UpnpEvent_New failed");
            break;
        }

        UpnpEvent_SetConnection(event, "Close");
        UpnpEvent_SetNt(event, "upnp:event");
        UpnpEvent_SetNts(event, "upnp:propchange");
        UpnpEvent_SetSeq(event, "0");

        count = UpnpService_GetStateVariableCount(service);
        for (i = 0; i < count; ++i)
        {
            UpnpStateVariable *v = (UpnpStateVariable *)UpnpService_GetStateVariableAt(service, i);
            if (v->sendEvents && v->isChanged)
            {
                const char *value = NULL;
                char buffer[128];

                memset(buffer, 0, 128);
                value = buffer;

                if (v->value.internalType == INTERNAL_STRING)
                {
                    value = v->value.internalValue.stringValue;
                }
                else
                {
                    if (RET_FAILED(DataValue_GetValue(&v->value, buffer, 128)))
                    {
                        LOG_E(TAG, "value invalid: %s", v->definition.name);
                        break;
                    }
                }

                UpnpEvent_SetArgumentValue(event, v->definition.name, value);
            }
        }

        if (UpnpEvent_GetArgumentCount(event) > 0)
        {
            count = UpnpService_GetSubscriberCount(service);
            for (i = 0; i < count; ++i)
            {
                UpnpEvent *job = NULL;
                UpnpSubscriber *s = (UpnpSubscriber *)UpnpService_GetSubscriberAt(service, i);
                UpnpEvent_SetCallback(event, UpnpSubscriber_GetCallback(s));
                UpnpEvent_SetSid(event, UpnpSubscriber_GetSid(s));

                job = UpnpEvent_New();
                UpnpEvent_Copy(job, event);
                TinyWorker_PutJob(&thiz->notifyWorker, job);
            }
        }

        UpnpEvent_Delete(event);
    } while (0);
}

UpnpGenaServer * UpnpGenaServer_New(UpnpHttpManager *http, UpnpProvider *provider)
{
    UpnpGenaServer *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpGenaServer *)tiny_malloc(sizeof(UpnpGenaServer));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpGenaServer_Construct(thiz, http, provider);
        if (RET_FAILED(ret))
        {
            UpnpGenaServer_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpGenaServer_Construct(UpnpGenaServer *thiz, UpnpHttpManager *http, UpnpProvider *provider)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpGenaServer));
        thiz->http = http;
        thiz->provider = provider;

        ret = UpnpHttpServer_RegisterSubscribeHandler(&http->server, OnSubscribe, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterSubscribeHandler: failed");
            break;
        }

        ret = UpnpHttpServer_RegisterUnsubscribeHandler(&http->server, OnUnsubscribe, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "UpnpHttpServer_RegisterUnsubscribeHandler: failed");
            break;
        }

        ret = TinyWorker_Construct(&thiz->notifyWorker);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyWorker_Construct: failed");
            break;
        }

        ret = TinyWorker_Initialize(&thiz->notifyWorker, OnNotifyJobDelete, thiz);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyWorker_Initialize: failed");
            break;
        }

        /** 
         * monitor provider
         */
        UpnpProvider_Lock(thiz->provider);

        do
        {
            ret = UpnpProvider_AddObserver(thiz->provider, "UpnpGenaServer", NULL, NULL, OnServiceChanged, thiz);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "UpnpProvider_AddObserver failed");
                break;
            }
        } while (0);

        UpnpProvider_Unlock(thiz->provider);
    } while (0);

    return ret;
}

void UpnpGenaServer_Dispose(UpnpGenaServer *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpProvider_Lock(thiz->provider);

    do
    {
        if (RET_FAILED(UpnpProvider_RemoveObserver(thiz->provider, "UpnpGenaServer")))
        {
            LOG_E(TAG, "UpnpProvider_RemoveObserver failed");
            break;
        }
    } while (0);

    UpnpProvider_Unlock(thiz->provider);

    if (RET_FAILED(UpnpHttpServer_UnregisterSubscribeHandler(&thiz->http->server)))
    {
        LOG_E(TAG, "UpnpHttpServer_UnregisterSubscriberHandler: failed");
    }

    if (RET_FAILED(UpnpHttpServer_UnregisterUnsubscribeHandler(&thiz->http->server)))
    {
        LOG_E(TAG, "UpnpHttpServer_UnregisterUnsubscriberHandler: failed");
    }

    TinyWorker_Dispose(&thiz->notifyWorker);

    thiz->http = NULL;
    thiz->provider = NULL;
}

void UpnpGenaServer_Delete(UpnpGenaServer *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpGenaServer_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpGenaServer_Start(UpnpGenaServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TinyWorker_Start(&thiz->notifyWorker, "Notifier", DoNotify, thiz);
}

TinyRet UpnpGenaServer_Stop(UpnpGenaServer *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TinyWorker_Stop(&thiz->notifyWorker);
}