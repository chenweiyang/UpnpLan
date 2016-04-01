/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpActionExecutor.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_ACTION_EXECUTOR_H__
#define __UPNP_ACTION_EXECUTOR_H__

#include "tiny_base.h"
#include "UpnpHttpManager.h"
#include "UpnpProvider.h"

TINY_BEGIN_DECLS


typedef struct _UpnpActionExecutor
{
    UpnpHttpManager *http;
    UpnpProvider *provider;
} UpnpActionExecutor;

UpnpActionExecutor * UpnpActionExecutor_New(UpnpHttpManager *http, UpnpProvider *provider);
TinyRet UpnpActionExecutor_Construct(UpnpActionExecutor *thiz, UpnpHttpManager *http, UpnpProvider *provider);
void UpnpActionExecutor_Dispose(UpnpActionExecutor *thiz);
void UpnpActionExecutor_Delete(UpnpActionExecutor *thiz);


TINY_END_DECLS

#endif /* __UPNP_ACTION_EXECUTOR_H__ */