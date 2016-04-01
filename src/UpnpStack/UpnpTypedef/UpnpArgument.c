/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpArgument.c
*
* @remark
*
*/

#include "UpnpArgument.h"
#include "tiny_memory.h"

static TinyRet UpnpArgument_Construct(UpnpArgument *thiz, const char *name, UpnpArgumentDirection direction, const char *relatedStateVariable);
static void UpnpArgument_Dispose(UpnpArgument *thiz);

#define UpnpArgument_NAME_LEN   128
#define VARIABLE_NAME_LEN   128

struct _UpnpArgument
{
    char name[UpnpArgument_NAME_LEN];
    UpnpArgumentDirection direction;
    char relatedStateVariable[VARIABLE_NAME_LEN];
};

UpnpArgument * UpnpArgument_New(const char *name, UpnpArgumentDirection direction, const char *relatedStateVariable)
{
    UpnpArgument *thiz = NULL;

    do
    {
        thiz = (UpnpArgument *)tiny_malloc(sizeof(UpnpArgument));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpArgument_Construct(thiz, name, direction, relatedStateVariable)))
        {
            UpnpArgument_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet UpnpArgument_Construct(UpnpArgument *thiz, const char *name, UpnpArgumentDirection direction, const char *relatedStateVariable)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpArgument));

        thiz->direction = direction;
        strncpy(thiz->name, name, UpnpArgument_NAME_LEN);
        strncpy(thiz->relatedStateVariable, relatedStateVariable, VARIABLE_NAME_LEN);
    } while (0);

    return ret;
}

static void UpnpArgument_Dispose(UpnpArgument *thiz)
{
    RETURN_IF_FAIL(thiz);
}

void UpnpArgument_Delete(UpnpArgument *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpArgument_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpArgument_SetName(UpnpArgument *thiz, const char *name)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    strncpy(thiz->name, name, UpnpArgument_NAME_LEN);

    return TINY_RET_OK;
}

TinyRet UpnpArgument_SetDirection(UpnpArgument *thiz, UpnpArgumentDirection direction)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    thiz->direction = direction;

    return TINY_RET_OK;
}

TinyRet UpnpArgument_SetRelatedStateVariable(UpnpArgument *thiz, const char *relatedStateVariable)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(relatedStateVariable, TINY_RET_E_ARG_NULL);

    strncpy(thiz->relatedStateVariable, relatedStateVariable, VARIABLE_NAME_LEN);

    return TINY_RET_OK;
}

const char * UpnpArgument_GetName(UpnpArgument *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->name;
}

UpnpArgumentDirection UpnpArgument_GetDirection(UpnpArgument *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, ARG_IN);

    return thiz->direction;
}

const char * UpnpArgument_GetRelatedStateVariable(UpnpArgument *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->relatedStateVariable;
}