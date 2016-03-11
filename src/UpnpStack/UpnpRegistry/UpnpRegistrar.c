/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpRegistrar.c
*
* @remark
*
*/

#include "UpnpRegistrar.h"
#include "tiny_memory.h"

static TinyRet UpnpRegistrar_Construct(UpnpRegistrar *thiz);
static TinyRet UpnpRegistrar_Dispose(UpnpRegistrar *thiz);

struct _UpnpRegistrar
{
    int i;
};

UpnpRegistrar * UpnpRegistrar_New(void)
{
    UpnpRegistrar *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpRegistrar *)tiny_malloc(sizeof(UpnpRegistrar));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpRegistrar_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpRegistrar_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet UpnpRegistrar_Construct(UpnpRegistrar *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpRegistrar));


    } while (0);

    return ret;
}

static TinyRet UpnpRegistrar_Dispose(UpnpRegistrar *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

void UpnpRegistrar_Delete(UpnpRegistrar *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpRegistrar_Dispose(thiz);
    tiny_free(thiz);
}

bool UpnpRegistrar_IsRegistered(UpnpRegistrar *thiz, UpnpDevice *device)
{
    RETURN_VAL_IF_FAIL(thiz, false);
    RETURN_VAL_IF_FAIL(device, false);

    return false;
}

TinyRet UpnpRegistrar_Register(UpnpRegistrar *thiz, UpnpDevice *device)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

TinyRet UpnpRegistrar_UnRegister(UpnpRegistrar *thiz, UpnpDevice *device)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(device, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}