/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpConnPool.c
 *
 * @remark
 *
 */

#include "TcpConnPool.h"
#include "tiny_memory.h"

static void ConnRemoveListener(void * data, void *ctx);
static bool ConnRemoveClosedListener(void * data, void *ctx);

TcpConnPool * TcpConnPool_New(void)
{
    TcpConnPool *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TcpConnPool *)tiny_malloc(sizeof(TcpConnPool));
        if (thiz == NULL)
        {
            break;
        }

        ret = TcpConnPool_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TcpConnPool_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet TcpConnPool_Construct(TcpConnPool *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TcpConnPool));

        ret = TinyList_Construct(&thiz->list);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->list, ConnRemoveListener, thiz);

        ret = TinyMutex_Construct(&thiz->mutex);
        if (RET_FAILED(ret))
        {
            break;
        }
    }
    while (0);

    return ret;
}

TinyRet TcpConnPool_Dispose(TcpConnPool *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinyMutex_Dispose(&thiz->mutex);
    TinyList_Dispose(&thiz->list);

    return TINY_RET_OK;
}

void TcpConnPool_Delete(TcpConnPool *thiz)
{
    RETURN_IF_FAIL(thiz);

    TcpConnPool_Dispose(thiz);
    tiny_free(thiz);
}

void TcpConnPool_Add(TcpConnPool *thiz, TcpConn *conn)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(conn);

    TinyMutex_Lock(&thiz->mutex);
    {
        TinyList_AddTail(&thiz->list, conn);
    }
    TinyMutex_Unlock(&thiz->mutex);
}

void TcpConnPool_Clean(TcpConnPool *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMutex_Lock(&thiz->mutex);
    {
        TinyList_RemoveAll(&thiz->list);
    }
    TinyMutex_Unlock(&thiz->mutex);
}

void TcpConnPool_RemoveClosed(TcpConnPool *thiz)
{
    TinyList_Foreach(&thiz->list, ConnRemoveClosedListener, (void *)thiz);
}

static void ConnRemoveListener (void * data, void *ctx)
{
    TcpConn *conn = (TcpConn *)data;
    TcpConn_Delete(conn);
}

static bool ConnRemoveClosedListener(void * data, void *ctx)
{
    TcpConn *conn = (TcpConn *)data;

    if (TcpConn_GetStatus(conn) == TCP_CONN_DISCONNECT)
    {
        TcpConn_Delete(conn);
    }

    return true;
}