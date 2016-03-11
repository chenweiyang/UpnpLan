/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObjectMessage.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpObjectMessage.h"
#include "tiny_memory.h"


UpnpObjectMessage * UpnpObjectMessage_New(void)
{
    UpnpObjectMessage *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpObjectMessage *)tiny_malloc(sizeof(UpnpObjectMessage));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpObjectMessage_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpObjectMessage_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet UpnpObjectMessage_Construct(UpnpObjectMessage *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpObjectMessage));
    }
    while (0);

    return ret;
}

TinyRet UpnpObjectMessage_Dispose(UpnpObjectMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

void UpnpObjectMessage_Delete(UpnpObjectMessage *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpObjectMessage_Dispose(thiz);
    tiny_free(thiz);
}

const char * UpnpObjectMessage_GetBuffer(UpnpObjectMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->buf;
}

const char * UpnpObjectMessage_GetIp(UpnpObjectMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->ip;
}

uint16_t UpnpObjectMessage_GetPort(UpnpObjectMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->port;
}

int UpnpObjectMessage_GetBytesRead(UpnpObjectMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->bytes_read;
}
