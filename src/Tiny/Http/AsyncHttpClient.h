/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   AsyncHttpClient.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __ASYNC_HTTP_CLIENT_H__
#define __ASYNC_HTTP_CLIENT_H__

#include "tiny_base.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


struct _AsyncHttpClient;
typedef struct _AsyncHttpClient AsyncHttpClient;

AsyncHttpClient * AsyncHttpClient_New(void);
TinyRet AsyncHttpClient_Construct(AsyncHttpClient *thiz);
TinyRet AsyncHttpClient_Dispose(AsyncHttpClient *thiz);
void AsyncHttpClient_Delete(AsyncHttpClient *thiz);

typedef void(*HttpClientListener)(AsyncHttpClient *client, 
    TinyRet result,
    HttpMessage *response, 
    void *ctx);

TinyRet AsyncHttpClient_Execute(AsyncHttpClient *thiz, 
    HttpMessage *request,
    uint32_t timeout,
    HttpClientListener listener,
    void *ctx);

bool AsyncHttpClient_IsConnected(AsyncHttpClient *thiz);
TinyRet AsyncHttpClient_Shutdown(AsyncHttpClient *thiz);


TINY_END_DECLS

#endif /* __ASYNC_HTTP_CLIENT_H__ */
