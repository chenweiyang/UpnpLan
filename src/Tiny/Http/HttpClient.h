/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   HttpClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "tiny_base.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


#define HTTP_DEBUG      1

struct _HttpClient;
typedef struct _HttpClient HttpClient;

HttpClient * HttpClient_New(void);
TinyRet HttpClient_Construct(HttpClient *thiz);
TinyRet HttpClient_Dispose(HttpClient *thiz);
void HttpClient_Delete(HttpClient *thiz);

TinyRet HttpClient_Execute(HttpClient *thiz, HttpMessage *request, HttpMessage *response, uint32_t timeout);
bool HttpClient_IsConnected(HttpClient *thiz);
TinyRet HttpClient_Shutdown(HttpClient *thiz);


TINY_END_DECLS

#endif /* __HTTP_CLIENT_H__ */
