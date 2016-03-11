/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpInitializer.h
*
* @remark
*
*/

#ifndef __UPNP_INITIALIZER_H__
#define __UPNP_INITIALIZER_H__

#include "tiny_base.h"
#include "upnp_api.h"

TINY_BEGIN_DECLS


UPNP_API void UpnpInitializer_Initialize(void);
UPNP_API void UpnpInitializer_Destroy(void);


TINY_END_DECLS

#endif /* __UPNP_INITIALIZER_H__ */