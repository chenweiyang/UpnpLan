/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   TcpConnPool.h
 *
 * @remark
 *
 */

#ifndef __TCP_CONN_POOL_H__
#define __TCP_CONN_POOL_H__

#include "tiny_base.h"
#include "TinyList.h"
#include "TinyMutex.h"
#include "TcpConn.h"

TINY_BEGIN_DECLS


typedef struct _TcpConnPool
{
    TinyList      list;
    TinyMutex     mutex;
} TcpConnPool;


TcpConnPool * TcpConnPool_New(void);
TinyRet TcpConnPool_Construct(TcpConnPool *thiz);
TinyRet TcpConnPool_Dispose(TcpConnPool *thiz);
void TcpConnPool_Delete(TcpConnPool *thiz);

void TcpConnPool_Add(TcpConnPool *thiz, TcpConn *conn);
void TcpConnPool_Clean(TcpConnPool *thiz);
void TcpConnPool_RemoveClosed(TcpConnPool *thiz);


TINY_END_DECLS

#endif /* __TCP_CONN_POOL_H__ */
