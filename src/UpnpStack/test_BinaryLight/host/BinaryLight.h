/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   BinaryLight.h
*
* @remark
*
*/

#ifndef __BINARY_LIGHT_H__
#define __BINARY_LIGHT_H__

#include "tiny_base.h"
#include "UpnpDeviceConfig.h"
#include "SwitchPower.h"
#include "UpnpRuntime.h"

TINY_BEGIN_DECLS


#define BINARYLIGHT_DEVICE_TYPE          "urn:schemas-upnp-org:device:BinaryLight:0.9"

struct _BinaryLight;
typedef struct _BinaryLight BinaryLight;

BinaryLight * BinaryLight_Create(UpnpDeviceConfig *config, UpnpRuntime *runtime);
void BinaryLight_Delete(BinaryLight *thiz);

const char * BinaryLight_GetDeviceType(BinaryLight *thiz);
SwitchPower * BinaryLight_GetSwitchPower(BinaryLight *thiz);

TinyRet BinaryLight_Start(BinaryLight *thiz);
TinyRet BinaryLight_Stop(BinaryLight *thiz);


TINY_END_DECLS

#endif /* __BINARY_LIGHT_H__ */
