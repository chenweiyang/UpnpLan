/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpActionInvoker.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_ACTION_INVOKER_H__
#define __UPNP_ACTION_INVOKER_H__

#include "tiny_base.h"
#include "upnp_define.h"
#include "UpnpAction.h"
#include "UpnpError.h"
#include "UpnpHttpManager.h"

TINY_BEGIN_DECLS


typedef struct _UpnpActionInvoker
{
    UpnpHttpManager     *http;
} UpnpActionInvoker;

UpnpActionInvoker * UpnpActionInvoker_New(UpnpHttpManager *http);
TinyRet UpnpActionInvoker_Construct(UpnpActionInvoker *thiz, UpnpHttpManager *http);
void UpnpActionInvoker_Dispose(UpnpActionInvoker *thiz);
void UpnpActionInvoker_Delete(UpnpActionInvoker *thiz);
TinyRet UpnpActionInvoker_Invoke(UpnpActionInvoker *thiz, UpnpAction *action, UpnpError *error);


TINY_END_DECLS

#endif /* __UPNP_ACTION_INVOKER_H__ */
