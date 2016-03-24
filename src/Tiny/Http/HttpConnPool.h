/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpConnPool.h
 *
 * @remark
 *
 */

#ifndef __HttpConnPool_H__
#define __HttpConnPool_H__

#include "tiny_base.h"
#include "HttpConn.h"
#include "HttpServer.h"

TINY_BEGIN_DECLS


struct _HttpConnPool;
typedef struct _HttpConnPool HttpConnPool;

// new & release
HttpConnPool * HttpConnPool_New(void);
HttpConnPool * HttpConnPool_clone(HttpConnPool * thiz);
void HttpConnPool_Delete(HttpConnPool *thiz);

// add & remove
void HttpConnPool_Add(HttpConnPool *thiz, HttpConn *conn);
void HttpConnPool_Remove(HttpConnPool *thiz, HttpConn *conn);

// clean
void HttpConnPool_Clean(HttpConnPool *thiz);
void HttpConnPool_CleanClosed(HttpConnPool *thiz);

// get count & item
int HttpConnPool_GetCount(HttpConnPool *thiz);
HttpConn * HttpConnPool_GetAt(HttpConnPool *thiz, uint32_t i);
HttpConn * HttpConnPool_GetBy(HttpConnPool *thiz, const char *ip, uint16_t port);


TINY_END_DECLS

#endif /* __HttpConnPool_H__ */
