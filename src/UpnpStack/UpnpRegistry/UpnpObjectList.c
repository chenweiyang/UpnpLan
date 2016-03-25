/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObjectList.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpObjectList.h"
#include "tiny_memory.h"

static void object_delete_listener(void * data, void *ctx)
{
    UpnpObject *object = (UpnpObject *)data;
    UpnpObject_Delete(object);
}

UpnpObjectList * UpnpObjectList_New(void)
{
    UpnpObjectList *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpObjectList *)tiny_malloc(sizeof(UpnpObjectList));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpObjectList_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpObjectList_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpObjectList_Construct(UpnpObjectList *thiz)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ret = TinyMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyMap_Construct(&thiz->objects);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->objects, object_delete_listener, NULL);
    } while (0);

    return ret;
}

TinyRet UpnpObjectList_Dispose(UpnpObjectList *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyMutex_Dispose(&thiz->mutex);
    TinyMap_Dispose(&thiz->objects);

    return TINY_RET_OK;
}

void UpnpObjectList_Delete(UpnpObjectList *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpObjectList_Dispose(thiz);
    tiny_free(thiz);
}

void UpnpObjectList_Lock(UpnpObjectList *thiz)
{
    TinyMutex_Lock(&thiz->mutex);
}

void UpnpObjectList_Unlock(UpnpObjectList *thiz)
{
    TinyMutex_Unlock(&thiz->mutex);
}

void UpnpObjectList_Clear(UpnpObjectList *thiz)
{
    TinyMap_Clear(&thiz->objects);
}

UpnpObject * UpnpObjectList_GetObject(UpnpObjectList *thiz, const char *usn)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(usn, NULL);

    return (UpnpObject *)TinyMap_GetValue(&thiz->objects, usn);
}

void UpnpObjectList_AddObject(UpnpObjectList *thiz, UpnpObject *object)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(object);

    TinyMap_Insert(&thiz->objects, UpnpObject_GetUsn(object), object);
}

void UpnpObjectList_RemoveObject(UpnpObjectList *thiz, const char *usn)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(usn);

    TinyMap_Erase(&thiz->objects, usn);
}