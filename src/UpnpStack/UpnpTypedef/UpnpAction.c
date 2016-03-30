/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpAction.c
*
* @remark
*
*/

#include "UpnpAction.h"
#include "tiny_memory.h"

static TinyRet UpnpAction_Construct(UpnpAction *thiz);
static void UpnpAction_Dispose(UpnpAction *thiz);

#define NAME_LEN    128

struct _UpnpAction
{
    char name[NAME_LEN];

    void * service;
    PropertyList * argumentList;
    PropertyList * resultList;
};

UpnpAction * UpnpAction_New(void)
{
    UpnpAction *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpAction *)tiny_malloc(sizeof(UpnpAction));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpAction_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpAction_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpAction_Delete(UpnpAction *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpAction_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpAction_Construct(UpnpAction *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpAction));

        thiz->argumentList = PropertyList_New();
        if (thiz->argumentList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        thiz->resultList = PropertyList_New();
        if (thiz->resultList == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }
    } while (0);

    return ret;
}

static void UpnpAction_Dispose(UpnpAction *thiz)
{
    RETURN_IF_FAIL(thiz);

    PropertyList_Delete(thiz->argumentList);
    PropertyList_Delete(thiz->resultList);
}

void UpnpAction_SetParentService(UpnpAction *thiz, void *service)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(service);

    thiz->service = service;
}

void* UpnpAction_GetParentService(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->service;
}

PropertyList* UpnpAction_GetArgumentList(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->argumentList;
}

PropertyList* UpnpAction_GetResultList(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->resultList;
}

TinyRet UpnpAction_SetName(UpnpAction *thiz, const char *name)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    strncpy(thiz->name, name, NAME_LEN);

    return TINY_RET_OK;
}

const char * UpnpAction_GetName(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->name;
}