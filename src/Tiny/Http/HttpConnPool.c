/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpConnPool.c
 *
 * @remark
 *
 */

#include "HttpConnPool.h"
#include "air_mutex.h"
#include "TinyThread.h"
#include "tiny_memory.h"

/*-----------------------------------------------------------------------------
 *
 * Private API declare
 *
 *-----------------------------------------------------------------------------*/

struct _HttpConnPool
{
    uint32_t                ref;

    DList                 * list;
    AirMutex              * mutex;
};


/*-----------------------------------------------------------------------------
 *
 * Public API
 *
 *-----------------------------------------------------------------------------*/

HttpConnPool * HttpConnPool_New(void)
{
    HttpConnPool *thiz = (HttpConnPool *)tiny_malloc(sizeof(HttpConnPool));
    if (thiz != NULL)
    {
        memset(thiz, 0, sizeof(HttpConnPool));
    
        thiz->list = dlist_new();
        if (thiz->list == NULL)
        {
            air_free(thiz);
            return NULL;
        }

        thiz->mutex = air_mutex_new();
        if (thiz->mutex == NULL)
        {
            dlist_release(thiz->list);
            air_free(thiz);
            return NULL;
        }

        thiz->ref = 1;
    }

    return thiz;
}

HttpConnPool * HttpConnPool_clone(HttpConnPool * thiz)
{
    return_val_if_fail(thiz, NULL);

    thiz->ref++;
    return thiz;
}

void HttpConnPool_Delete(HttpConnPool *thiz)
{
    return_if_fail(thiz);
    
    if (thiz->ref > 1)
    {
        thiz->ref --;
        return;
    }

    HttpConnPool_Clean(thiz);

    dlist_release(thiz->list);
    air_mutex_release(thiz->mutex);

    air_free(thiz);
}

void HttpConnPool_Add(HttpConnPool *thiz, HttpConn *conn)
{
    return_if_fail(thiz);
    return_if_fail(conn);

    air_mutex_lock(thiz->mutex);

    dlist_add_tail(thiz->list, conn);

    air_mutex_unlock(thiz->mutex);
}

void HttpConnPool_Clean(HttpConnPool *thiz)
{
    HttpConn *conn = NULL;

    return_if_fail(thiz);

    air_mutex_lock(thiz->mutex);

    conn = (HttpConn *)dlist_get_head(thiz->list);
    while (conn)
    {
        dlist_remove_head(thiz->list);
        HttpConn_Delete(conn);
        conn = (HttpConn *)dlist_get_head(thiz->list);
    }

    air_mutex_unlock(thiz->mutex);
}

void HttpConnPool_CleanClosed(HttpConnPool *thiz)
{
    int index = 0;
    HttpConn *conn = NULL;

    return_if_fail(thiz);

    air_mutex_lock(thiz->mutex);

    conn = (HttpConn *)dlist_get_head(thiz->list);
    while (conn)
    {
        if (HttpConn_GetStatus(conn) == HTTP_CONN_DISCONNECT)
        {
            dlist_remove_at(thiz->list, index);
            HttpConn_Delete(conn);
        }
        else
        {
            index ++;
        }

        conn = (HttpConn *)dlist_get_at(thiz->list, index);
    }

    air_mutex_unlock(thiz->mutex);
}

int HttpConnPool_GetCount(HttpConnPool *thiz)
{
    int count = 0;

    return_val_if_fail(thiz, -1);

    air_mutex_lock(thiz->mutex);

    count = dlist_get_count(thiz->list);

    air_mutex_unlock(thiz->mutex);

    return count;
}

HttpConn * HttpConnPool_GetAt(HttpConnPool *thiz, uint32_t i)
{
    HttpConn *conn = NULL;

    return_val_if_fail(thiz, NULL);

    air_mutex_lock(thiz->mutex);
    conn = (HttpConn *)dlist_get_at(thiz->list, i);
    air_mutex_unlock(thiz->mutex);

    return conn;
}

HttpConn * HttpConnPool_GetBy(HttpConnPool *thiz, const char *ip, uint16_t port)
{
    int i = 0;
    int count = 0;
    HttpConn *conn = NULL;
    const char *conn_ip = NULL;
    uint16_t conn_port = 0;

    return_val_if_fail(thiz, NULL);
    return_val_if_fail(ip, NULL);

    air_mutex_lock(thiz->mutex);

    for (i = 0; i < count; ++i)
    {
        conn = (HttpConn *)dlist_get_at(thiz->list, i);
        conn_ip = HttpConn_GetClientAddress(conn);
        conn_port = HttpConn_GetClientPort(conn);

        if ((strcmp(ip, conn_ip) == 0) && (port == conn_port))
        {
            air_mutex_unlock(thiz->mutex);
            return conn;
        }
    }

    air_mutex_unlock(thiz->mutex);

    return NULL;
}