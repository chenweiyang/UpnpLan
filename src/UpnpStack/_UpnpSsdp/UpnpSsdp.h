/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSsdp.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_SSDP_H__
#define __UPNP_SSDP_H__

#include "ct_common.h"
#include "UpnpObject.h"
#include "UpnpUsn.h"
#include "CtThread.h"
#include "CtMap.h"
#include "CtWorker.h"
#include "CtSelector.h"
#include "CtSocketIpc.h"

CT_BEGIN_DECLS


struct _UpnpSsdp;
typedef struct _UpnpSsdp UpnpSsdp;

UpnpSsdp * UpnpSsdp_GetInstance();
CtRet UpnpSsdp_SetUuidMode(UpnpSsdp *thiz, bool strict_uuid);

typedef bool (*UpnpObjectFilter)(UpnpUsn *usn, void *ctx);
typedef void (*UpnpObjectListener)(UpnpObject *objects, bool alive, void *ctx);
CtRet UpnpSsdp_SetListener(UpnpSsdp *thiz, UpnpObjectListener listener, UpnpObjectFilter filter, void *ctx);

CtRet UpnpSsdp_Start(UpnpSsdp *thiz);
CtRet UpnpSsdp_Stop(UpnpSsdp *thiz);
CtRet UpnpSsdp_Search(UpnpSsdp *thiz);

CtRet UpnpSsdp_StartAdvertising(UpnpSsdp *thiz, UpnpObject *object);
CtRet UpnpSsdp_StopAdvertising(UpnpSsdp *thiz, UpnpObject *object);


CT_END_DECLS

#endif /* __UPNP_SSDP_H__ */
