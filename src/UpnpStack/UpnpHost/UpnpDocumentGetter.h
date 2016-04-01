/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpDocumentGetter.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_DOCUMENT_GETTER_H__
#define __UPNP_DOCUMENT_GETTER_H__

#include "tiny_base.h"
#include "UpnpHttpManager.h"
#include "UpnpProvider.h"

TINY_BEGIN_DECLS


typedef struct _UpnpDocumentGetter
{
    UpnpHttpManager *http;
    UpnpProvider *provider;
} UpnpDocumentGetter;

UpnpDocumentGetter * UpnpDocumentGetter_New(UpnpHttpManager *http, UpnpProvider *provider);
TinyRet UpnpDocumentGetter_Construct(UpnpDocumentGetter *thiz, UpnpHttpManager *http, UpnpProvider *provider);
void UpnpDocumentGetter_Dispose(UpnpDocumentGetter *thiz);
void UpnpDocumentGetter_Delete(UpnpDocumentGetter *thiz);


TINY_END_DECLS

#endif /* __UPNP_DOCUMENT_GETTER_H__ */