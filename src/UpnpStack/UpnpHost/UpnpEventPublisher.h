/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpEventPublisher.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_EVENT_PUBLISHER_H__
#define __UPNP_EVENT_PUBLISHER_H__

#include "tiny_base.h"
#include "UpnpHttpManager.h"
#include "UpnpProvider.h"

TINY_BEGIN_DECLS


typedef struct _UpnpEventPublisher
{
    UpnpHttpManager *http;
    UpnpProvider *provider;
} UpnpEventPublisher;

UpnpEventPublisher * UpnpEventPublisher_New(UpnpHttpManager *http, UpnpProvider *provider);
TinyRet UpnpEventPublisher_Construct(UpnpEventPublisher *thiz, UpnpHttpManager *http, UpnpProvider *provider);
void UpnpEventPublisher_Dispose(UpnpEventPublisher *thiz);
void UpnpEventPublisher_Delete(UpnpEventPublisher *thiz);


TINY_END_DECLS

#endif /* __UPNP_EVENT_PUBLISHER_H__ */