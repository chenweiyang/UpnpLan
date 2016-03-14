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

TINY_BEGIN_DECLS


struct _UpnpActionInvoker;
typedef struct _UpnpActionInvoker UpnpActionInvoker;

UpnpActionInvoker * UpnpActionInvoker_New(void);
void UpnpActionInvoker_Delete(UpnpActionInvoker *thiz);

TinyRet UpnpActionInvoker_Invoke(UpnpActionInvoker *thiz, UpnpAction *action, UpnpError *error);


TINY_END_DECLS

#endif /* __UPNP_ACTION_INVOKER_H__ */
