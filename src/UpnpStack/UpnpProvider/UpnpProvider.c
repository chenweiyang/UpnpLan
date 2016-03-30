/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpProvider.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpProvider.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "UpnpDeviceParser.h"
#include "UpnpServiceParser.h"

#define TAG     "UpnpProvider"

static uint32_t UpnpProvider_GetServiceDocument(UpnpProvider *thiz, UpnpDevice *device, const char *uri, char *content, uint32_t len);

static void device_delete_listener(void * data, void *ctx)
{
    // NOTHING TO DO !
}

static void handler_delete_listener(void *data, void *ctx)
{
    // NOTHING TO DO !
}

static void observer_delete_listener(void *data, void *ctx)
{
    UpnpObserver * observer = (UpnpObserver *)data;
    UpnpObserver_Delete(observer);
}

static void onServiceChanged(UpnpService *service, void *ctx)
{
    UpnpProvider *thiz = (UpnpProvider *)ctx;
    uint32_t count = TinyMap_GetCount(&thiz->observers);
    uint32_t i = 0;

    /**
     * Notify
     */
    for (i = 0; i < count; i++)
    {
        UpnpObserver *observer = (UpnpObserver *)TinyMap_GetValueAt(&thiz->observers, i);
        if (observer->OnServiceChanged != NULL)
        {
            observer->OnServiceChanged(service, observer->ctx);
        }
    }
}

UpnpProvider * UpnpProvider_New(void)
{
    UpnpProvider *thiz = NULL;

    do
    {
        thiz = (UpnpProvider *)tiny_malloc(sizeof(UpnpProvider));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpProvider_Construct(thiz)))
        {
            UpnpProvider_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpProvider_Construct(UpnpProvider *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpProvider));

        ret = TinyMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            break;
        }

        /**
         * devices
         */
        ret = TinyMap_Construct(&thiz->devices);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->devices, device_delete_listener, NULL);

        /**
         * handlers
         */
        ret = TinyMap_Construct(&thiz->handlers);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->handlers, handler_delete_listener, NULL);

        /**
         * observers
         */
        ret = TinyMap_Construct(&thiz->observers);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->observers, observer_delete_listener, NULL);
    } while (0);

    return ret;
}

TinyRet UpnpProvider_Dispose(UpnpProvider *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyMutex_Dispose(&thiz->mutex);
    TinyMap_Dispose(&thiz->handlers);
    TinyMap_Dispose(&thiz->devices);

    return TINY_RET_OK;
}

void UpnpProvider_Delete(UpnpProvider *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpProvider_Dispose(thiz);
    tiny_free(thiz);
}

void UpnpProvider_Lock(UpnpProvider *thiz)
{
    TinyMutex_Lock(&thiz->mutex);
}

void UpnpProvider_Unlock(UpnpProvider *thiz)
{
    TinyMutex_Unlock(&thiz->mutex);
}

TinyRet UpnpProvider_AddObserver(UpnpProvider *thiz,
    const char *name,
    DeviceAddedListener OnDeviceAdded,
    DeviceRemovedListener OnDeviceRemoved,
    ServiceChangedListener OnServiceChanged,
    void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t count = TinyMap_GetCount(&thiz->devices);
        uint32_t i = 0;

        UpnpObserver *observer = UpnpObserver_New();
        if (observer == NULL)
        {
            LOG_E(TAG, "UpnpObserver_New failed");
            ret = TINY_RET_E_NEW;
            break;
        }

        observer->OnDeviceAdded = OnDeviceAdded;
        observer->OnDeviceRemoved = OnDeviceRemoved;
        observer->OnServiceChanged = OnServiceChanged;
        observer->ctx = ctx;

        ret = TinyMap_Insert(&thiz->observers, name, observer);
        if (RET_FAILED(ret))
        {
            UpnpObserver_Delete(observer);
            break;
        }

        /**
         * Notify
         */
        if (OnDeviceAdded != NULL)
        {
            for (i = 0; i < count; i++)
            {
                UpnpDevice * device = (UpnpDevice *)TinyMap_GetValueAt(&thiz->devices, i);
                OnDeviceAdded(device, ctx);
            }
        }
    } while (0);

    return ret;
}

TinyRet UpnpProvider_RemoveObserver(UpnpProvider *thiz, const char *name)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t count = TinyMap_GetCount(&thiz->devices);
        uint32_t i = 0;

        UpnpObserver *observer = (UpnpObserver *)TinyMap_GetValue(&thiz->observers, name);
        if (observer == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        /**
         * Notify
         */
        if (observer->OnDeviceRemoved != NULL)
        {
            for (i = 0; i < count; i++)
            {
                UpnpDevice * device = (UpnpDevice *)TinyMap_GetValueAt(&thiz->devices, i);
                observer->OnDeviceRemoved(device, observer->ctx);
            }
        }

        ret = TinyMap_Erase(&thiz->observers, name);

    } while (0);

    return ret;
}

void UpnpProvider_Foreach(UpnpProvider *thiz, const char *st, DeviceVisitor OnDeviceVisit, void *ctx)
{
    uint32_t count = TinyMap_GetCount(&thiz->devices);
    uint32_t i = 0;

    for (i = 0; i < count; i++)
    {
        UpnpDevice * device = (UpnpDevice *)TinyMap_GetValueAt(&thiz->devices, i);
        OnDeviceVisit(device, ctx);
    }
}

void UpnpProvider_Clear(UpnpProvider *thiz)
{
     TinyMap_Clear(&thiz->devices);
}

UpnpDevice * UpnpProvider_GetDevice(UpnpProvider *thiz, const char *deviceId)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(deviceId, NULL);

    return (UpnpDevice *)TinyMap_GetValue(&thiz->devices, deviceId);
}

TinyRet UpnpProvider_Add(UpnpProvider *thiz, UpnpDevice *device, UpnpActionHandler handler, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpServiceList *list = UpnpDevice_GetServiceList(device);
        uint32_t count = TinyMap_GetCount(&thiz->observers);
        uint32_t i = 0;

        /**
         * add device  & handler
         */
        const char *deviceId = UpnpDevice_GetDeviceId(device);
        if (deviceId == NULL)
        {
            LOG_D(TAG, "DeviceId is NULL");
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        ret = TinyMap_Insert(&thiz->devices, deviceId, device);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyMap_Insert(&thiz->handlers, deviceId, handler);
        if (RET_FAILED(ret))
        {
            break;
        }

        /**
         * Notify
         */
        for (i = 0; i < count; i++)
        {
            UpnpObserver *observer = (UpnpObserver *)TinyMap_GetValueAt(&thiz->observers, i);
            if (observer->OnDeviceAdded != NULL)
            {
                observer->OnDeviceAdded(device, observer->ctx);
            }
        }

        /**
         * Register ServiceChangedHandler
         */
        count = UpnpServiceList_GetSize(list);
        for (i = 0; i < count; ++i)
        {
            UpnpService *service = (UpnpService *)UpnpServiceList_GetServiceAt(list, i);
            UpnpService_SetChangedListener(service, onServiceChanged, thiz);
        }
    } while (0);

    return ret;
}

TinyRet UpnpProvider_Remove(UpnpProvider *thiz, const char *deviceId)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(deviceId, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpServiceList *list = NULL;
        uint32_t count = TinyMap_GetCount(&thiz->observers);
        uint32_t i = 0;

        /**
         * Notify
         */
        UpnpDevice *device = (UpnpDevice *)TinyMap_GetValue(&thiz->devices, deviceId);
        if (device == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        for (i = 0; i < count; i++)
        {
            UpnpObserver *observer = (UpnpObserver *)TinyMap_GetValueAt(&thiz->observers, i);
            if (observer->OnDeviceRemoved != NULL)
            {
                observer->OnDeviceRemoved(device, observer->ctx);
            }
        }

        /**
        * UnRegister ServiceChangedHandler
        */
        list = UpnpDevice_GetServiceList(device);
        count = UpnpServiceList_GetSize(list);
        for (i = 0; i < count; ++i)
        {
            UpnpService *service = (UpnpService *)UpnpServiceList_GetServiceAt(list, i);
            UpnpService_SetChangedListener(service, NULL, NULL);
        }

        /**
         * delete device & handler
         */
        ret = TinyMap_Erase(&thiz->devices, deviceId);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyMap_Erase(&thiz->handlers, deviceId);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

uint32_t UpnpProvider_GetDocument(UpnpProvider *thiz, const char *uri, char *content, uint32_t len)
{
    uint32_t ret = 0;

    RETURN_VAL_IF_FAIL(thiz, 0);
    RETURN_VAL_IF_FAIL(content, 0);

    do
    {
        uint32_t i = 0;
        uint32_t count = TinyMap_GetCount(&thiz->devices);

        for (i = 0; i < count; i++)
        {
            UpnpDevice *device = (UpnpDevice *)TinyMap_GetValueAt(&thiz->devices, i);

            if (STR_EQUAL(uri, UpnpDevice_GetURI(device)))
            {
                ret = UpnpDeviceParser_ToXml(device, content, len);
                break;
            }

            ret = UpnpProvider_GetServiceDocument(thiz, device, uri, content, len);
            if (ret > 0)
            {
                break;
            }
        }
    } while (0);

    return ret;
}

static uint32_t UpnpProvider_GetServiceDocument(UpnpProvider *thiz, UpnpDevice *device, const char *uri, char *content, uint32_t len)
{
    uint32_t ret = 0;

    RETURN_VAL_IF_FAIL(thiz, 0);
    RETURN_VAL_IF_FAIL(content, 0);

    do
    {
        UpnpServiceList * list = UpnpDevice_GetServiceList(device);
        uint32_t count = UpnpServiceList_GetSize(list);
        uint32_t i = 0;

        for (i = 0; i < count; i++)
        {
            UpnpService *service = UpnpServiceList_GetServiceAt(list, i);
            const char *scpd = UpnpService_GetSCPDURL(service);

            if (STR_EQUAL(uri, scpd))
            {
                ret = UpnpServiceParser_ToXml(service, content, len);
                break;
            }
        }
    } while (0);

    return ret;
}