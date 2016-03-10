/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpStateList.h
*
* @remark
*
*/

#include "UpnpStateList.h"
#include "TinyMap.h"
#include "tiny_memory.h"

static TinyRet UpnpStateList_Construct(UpnpStateList *thiz);
static void UpnpStateList_Dispose(UpnpStateList *thiz);
static void UpnpStateDeleteListener(void * data, void *ctx);

struct _UpnpStateList
{
    TinyMap      states;
};

UpnpStateList * UpnpStateList_New(void)
{
    UpnpStateList *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpStateList *)tiny_malloc(sizeof(UpnpStateList));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpStateList_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpStateList_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpStateList_Delete(UpnpStateList * thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpStateList_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpStateList_Construct(UpnpStateList *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpStateList));

        ret = TinyMap_Construct(&thiz->states);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->states, UpnpStateDeleteListener, thiz);
    } while (0);

    return ret;
}

static void UpnpStateList_Dispose(UpnpStateList *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMap_Dispose(&thiz->states);
}

static void UpnpStateDeleteListener(void * data, void *ctx)
{
    UpnpState *state = (UpnpState *)data;
    tiny_free(state);
}

TinyRet UpnpStateList_InitState(UpnpStateList *thiz, const char *name, ObjectType *type, bool sendEvents, void *service)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        UpnpState *state = UpnpState_New();
        UpnpState_Initialize(state, name, type, NULL, sendEvents, service);

        ret = TinyMap_Insert(&thiz->states, name, state);
        if (RET_FAILED(ret))
        {
            UpnpState_Delete(state);
        }
    } while (0);

    return ret;
}

TinyRet UpnpStateList_SetStateValue(UpnpStateList *thiz, const char *name, Object *data)
{
    TinyRet ret = TINY_RET_OK;
    UpnpState *state = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(data, TINY_RET_E_ARG_NULL);

    do
    {
        state = TinyMap_GetValue(&thiz->states, name);
        if (state == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (state->definition.type.clazzType != data->type.clazzType)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_Copy(&state->value.object, data);
    } while (0);

    return ret;
}

Object * UpnpStateList_GetUpnpStateValue(UpnpStateList *thiz, const char *name)
{
    Object * data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    do
    {
        UpnpState *state = (UpnpState *)TinyMap_GetValue(&thiz->states, name);
        if (state == NULL)
        {
            break;
        }

        data = &state->value.object;
    } while (0);

    return data;
}

uint32_t UpnpStateList_GetSize(UpnpStateList *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyMap_GetSize(&thiz->states);
}

UpnpState * UpnpStateList_GetStateAt(UpnpStateList *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpState *)TinyMap_GetValueAt(&thiz->states, index);
}

UpnpState * UpnpStateList_GetState(UpnpStateList *thiz, const char *name)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    return (UpnpState *)TinyMap_GetValue(&thiz->states, name);
}