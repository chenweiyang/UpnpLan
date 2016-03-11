/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSearcher.c
*
* @remark
*
*/

#include "UpnpSearcher.h"
#include "tiny_memory.h"

static TinyRet UpnpSearcher_Construct(UpnpSearcher *thiz);
static TinyRet UpnpSearcher_Dispose(UpnpSearcher *thiz);

struct _UpnpSearcher
{
    int i;
};

UpnpSearcher * UpnpSearcher_New(void)
{
    UpnpSearcher *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpSearcher *)tiny_malloc(sizeof(UpnpSearcher));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpSearcher_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpSearcher_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet UpnpSearcher_Construct(UpnpSearcher *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSearcher));

    } while (0);

    return ret;
}

static TinyRet UpnpSearcher_Dispose(UpnpSearcher *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

void UpnpSearcher_Delete(UpnpSearcher *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpSearcher_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpSearcher_StartScan(UpnpSearcher *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

TinyRet UpnpSearcher_StopScan(UpnpSearcher *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}