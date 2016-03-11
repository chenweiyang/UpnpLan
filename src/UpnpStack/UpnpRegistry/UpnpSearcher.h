/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSearcher.h
*
* @remark
*
*/

#ifndef __UPNP_SEARCHER_H__
#define __UPNP_SEARCHER_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpUsn.h"
#include "UpnpDevice.h"

TINY_BEGIN_DECLS


struct _UpnpSearcher;
typedef struct _UpnpSearcher UpnpSearcher;

UPNP_API UpnpSearcher * UpnpSearcher_New(void);
UPNP_API void UpnpSearcher_Delete(UpnpSearcher *thiz);

typedef void(*UpnpSearcherListener)(UpnpDevice *device, bool alive, void *ctx);
UPNP_API TinyRet UpnpSearcher_StartScan(UpnpSearcher *thiz);
UPNP_API TinyRet UpnpSearcher_StopScan(UpnpSearcher *thiz);


TINY_END_DECLS

#endif /* __UPNP_SEARCHER_H__ */