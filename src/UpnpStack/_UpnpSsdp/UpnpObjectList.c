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
#include "CtMutex.h"
#include "CtMap.h"
#include "ct_memory.h"

struct _UpnpObjectList
{
    CtMutex     mutex;
    CtMap       objects;
};

static void object_delete_listener(void * data, void *ctx)
{
    UpnpObject *object = (UpnpObject *)data;
    UpnpObject_Delete(object);
}

static CtRet UpnpObjectList_Construct(UpnpObjectList *thiz)
{
    CtRet ret = CT_RET_OK;

    do
    {
        ret = CtMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = CtMap_Construct(&thiz->objects);
        if (RET_FAILED(ret))
        {
            break;
        }

        CtMap_SetDeleteListener(&thiz->objects, object_delete_listener, NULL);
    } while (0);

    return ret;
}

static CtRet UpnpObjectList_Dispose(UpnpObjectList *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    CtMutex_Dispose(&thiz->mutex);
    CtMap_Dispose(&thiz->objects);

    return CT_RET_OK;
}

UpnpObjectList * UpnpObjectList_New(void)
{
    UpnpObjectList *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpObjectList *)ct_malloc(sizeof(UpnpObjectList));
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

void UpnpObjectList_Delete(UpnpObjectList *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpObjectList_Dispose(thiz);
    ct_free(thiz);
}

void UpnpObjectList_Lock(UpnpObjectList *thiz)
{
    CtMutex_Lock(&thiz->mutex);
}

void UpnpObjectList_Unlock(UpnpObjectList *thiz)
{
    CtMutex_Unlock(&thiz->mutex);
}

void UpnpObjectList_Clear(UpnpObjectList *thiz)
{
    CtMutex_Lock(&thiz->mutex);
    CtMap_Clear(&thiz->objects);
    CtMutex_Unlock(&thiz->mutex);
}

UpnpObject * UpnpObjectList_GetObject(UpnpObjectList *thiz, const char *usn)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(usn, NULL);

    return (UpnpObject *)CtMap_GetValue(&thiz->objects, usn);
}

void UpnpObjectList_AddObject(UpnpObjectList *thiz, UpnpObject *object)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(object);

    CtMap_Insert(&thiz->objects, UpnpObject_GetUsn(object), object);
}

void UpnpObjectList_RemoveObject(UpnpObjectList *thiz, const char *usn)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(usn);

    CtMap_Erase(&thiz->objects, usn);
}