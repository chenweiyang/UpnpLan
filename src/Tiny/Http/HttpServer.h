/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpServer.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __HTTP_SERVERP_H__
#define __HTTP_SERVERP_H__

#include "tiny_base.h"
#include "HttpConn.h"

TINY_BEGIN_DECLS


struct _HttpServer;
typedef struct _HttpServer HttpServer;

// net & release
HttpServer * HttpServer_New(void);
HttpServer * HttpServer_Clone(HttpServer *thiz);
void HttpServer_Release(HttpServer *thiz);

// start & stop
TinyRet HttpServer_Start(HttpServer *thiz, uint16_t port, HttpConnListener listener, void *ctx);
TinyRet HttpServer_Stop(HttpServer *thiz);

// listen port
uint16_t HttpServer_GetListeningPort(HttpServer *thiz);


TINY_END_DECLS


#endif /* __AIR_HTTP_SERVERP_H__ */