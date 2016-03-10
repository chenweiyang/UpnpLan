/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSsdpJob.c
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#include "UpnpSsdpJob.h"
#include "ct_memory.h"


UpnpSsdpJob * UpnpSsdpJob_New(void)
{
    UpnpSsdpJob *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpSsdpJob *)ct_malloc(sizeof(UpnpSsdpJob));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpSsdpJob_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpSsdpJob_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

CtRet UpnpSsdpJob_Construct(UpnpSsdpJob *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSsdpJob));
    }
    while (0);

    return ret;
}

CtRet UpnpSsdpJob_Dispose(UpnpSsdpJob *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    return CT_RET_OK;
}

void UpnpSsdpJob_Delete(UpnpSsdpJob *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpSsdpJob_Dispose(thiz);
    ct_free(thiz);
}

const char * UpnpSsdpJob_GetBuffer(UpnpSsdpJob *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->buf;
}

const char * UpnpSsdpJob_GetIp(UpnpSsdpJob *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->ip;
}

uint16_t UpnpSsdpJob_GetPort(UpnpSsdpJob *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->port;
}

int UpnpSsdpJob_GetBytesRead(UpnpSsdpJob *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->bytes_read;
}