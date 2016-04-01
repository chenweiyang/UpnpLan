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
#include "TinyList.h"

static TinyRet UpnpAction_Construct(UpnpAction *thiz);
static void UpnpAction_Dispose(UpnpAction *thiz);
static void UpnpArgumentDeleteListener (void * data, void *ctx)
{
    UpnpArgument *argument = (UpnpArgument *)data;
    UpnpArgument_Delete(argument);
}

#define NAME_LEN    128


struct _UpnpAction
{
    void * service;
    char name[NAME_LEN];
    TinyList argumentList;
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

        ret = TinyList_Construct(&thiz->argumentList);
        if (RET_FAILED(ret))
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        TinyList_SetDeleteListener(&thiz->argumentList, UpnpArgumentDeleteListener, thiz);
    } while (0);

    return ret;
}

static void UpnpAction_Dispose(UpnpAction *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyList_Dispose(&thiz->argumentList);
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

TinyRet UpnpAction_AddArgument(UpnpAction *thiz, UpnpArgument *argument)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(argument, TINY_RET_E_ARG_NULL);

    return TinyList_AddTail(&thiz->argumentList, argument);
}

uint32_t UpnpAction_GetArgumentCount(UpnpAction *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyList_GetSize(&thiz->argumentList);
}

UpnpArgument * UpnpAction_GetArgumentAt(UpnpAction *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpArgument *)TinyList_GetAt(&thiz->argumentList, index);
}

UpnpArgument * UpnpAction_GetArgument(UpnpAction *thiz, const char *argumentName)
{
    uint32_t i = 0;
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(argumentName, NULL);

    count = TinyList_GetCount(&thiz->argumentList);

    for (i = 0; i < count; ++i)
    {
        UpnpArgument *argument = (UpnpArgument *)TinyList_GetAt(&thiz->argumentList, i);
        if (STR_EQUAL(UpnpArgument_GetName(argument), argumentName))
        {
            return argument;
        }
    }

    return NULL;
}

const char * UpnpAction_GetArgumentRelatedStateVariable(UpnpAction *thiz, const char *argumentName)
{
    UpnpArgument * argument = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(argumentName, NULL);

    argument = UpnpAction_GetArgument(thiz, argumentName);
    if (argument != NULL)
    {
        return UpnpArgument_GetRelatedStateVariable(argument);
    }

    return NULL;
}