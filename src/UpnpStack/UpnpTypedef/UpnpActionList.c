/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpActionList.c
*
* @remark
*
*/

#include "UpnpActionList.h"
#include "UpnpActionDefinition.h"
#include "TinyMap.h"
#include "tiny_memory.h"

static TinyRet UpnpActionList_Construct(UpnpActionList *thiz);
static void UpnpActionList_Dispose(UpnpActionList *thiz);
static void ActionDeleteListener(void * data, void *ctx);

struct _ActionList
{
    TinyMap      actions;
};

UpnpActionList * UpnpActionList_New(void)
{
    UpnpActionList *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpActionList *)tiny_malloc(sizeof(UpnpActionList));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpActionList_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpActionList_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void UpnpActionList_Delete(UpnpActionList * thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpActionList_Dispose(thiz);
    tiny_free(thiz);
}

static TinyRet UpnpActionList_Construct(UpnpActionList *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpActionList));

        ret = TinyMap_Construct(&thiz->actions);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->actions, ActionDeleteListener, thiz);
    } while (0);

    return ret;
}

static void UpnpActionList_Dispose(UpnpActionList *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMap_Dispose(&thiz->actions);
}

static void ActionDeleteListener(void * data, void *ctx)
{
    UpnpAction *action = (UpnpAction *)data;
    UpnpAction_Delete(action);
}

TinyRet UpnpActionList_AddAction(UpnpActionList * thiz, UpnpAction *action)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(action, TINY_RET_E_ARG_NULL);

    return TinyMap_Insert(&thiz->actions, UpnpAction_GetPropertyValue(action, UPNP_ACTION_Name), action);
}

uint32_t UpnpActionList_GetSize(UpnpActionList *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyMap_GetSize(&thiz->actions);
}

UpnpAction * UpnpActionList_GetActionAt(UpnpActionList *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpAction *)TinyMap_GetValueAt(&thiz->actions, index);
}

UpnpAction * UpnpActionList_GetAction(UpnpActionList *thiz, const char *actionName)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (UpnpAction *)TinyMap_GetValue(&thiz->actions, actionName);
}