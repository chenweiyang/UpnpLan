/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpObjectFactory.h
*
* @remark
*
*/

#ifndef __UPNP_OBJECT_FACTORY_H__
#define __UPNP_OBJECT_FACTORY_H__

#include "tiny_base.h"
#include "UpnpObject.h"
#include "SsdpMessage.h"
#include "UpnpValidator.h"

TINY_BEGIN_DECLS


UpnpObject * UpnpObjectFactory_CreateByAlive(SsdpAlive *alive, const char *ip, UpnpValidator *validator);
UpnpObject * UpnpObjectFactory_CreateByResponse(SsdpResponse *response, const char *ip, UpnpValidator *validator);


TINY_END_DECLS

#endif /* __UPNP_OBJECT_FACTORY_H__ */