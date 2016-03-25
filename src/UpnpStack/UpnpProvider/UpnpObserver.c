/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObserver.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpObserver.h"
#include "tiny_memory.h"
#include "tiny_log.h"

UpnpObserver * UpnpObserver_New(void)
{
    UpnpObserver *thiz = NULL;

    do
    {
        thiz = (UpnpObserver *)tiny_malloc(sizeof(UpnpObserver));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(UpnpObserver_Construct(thiz)))
        {
            UpnpObserver_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpObserver_Construct(UpnpObserver *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpObserver));
    } while (0);

    return ret;
}

TinyRet UpnpObserver_Dispose(UpnpObserver *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

void UpnpObserver_Delete(UpnpObserver *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpObserver_Dispose(thiz);
    tiny_free(thiz);
}