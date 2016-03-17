/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   SwitchPower.h
*
* @remark
*
*/

#ifndef __SWITCH_POWER_H__
#define __SWITCH_POWER_H__

#include "tiny_base.h"
#include "UpnpDevice.h"
#include "UpnpCode.h"
#include "UpnpRuntime.h"

TINY_BEGIN_DECLS


struct _SwitchPower;
typedef struct _SwitchPower SwitchPower;

SwitchPower * SwitchPower_Create(UpnpDevice *device, UpnpRuntime *runtime);
void SwitchPower_Delete(SwitchPower *thiz);
bool SwitchPower_IsImplemented(SwitchPower *thiz, UpnpService *service);

/**
 * Action: OnGetTarget
 */
typedef struct _SwitchPower_GetTargetResult
{
    bool theTargetValue;
} SwitchPower_GetTargetResult;

typedef UpnpCode (* SwitchPower_OnGetTarget)(SwitchPower *thiz, SwitchPower_GetTargetResult *result, void *ctx);

/**
 * Action: OnSetTarget
 */
typedef UpnpCode (* SwitchPower_OnSetTarget)(SwitchPower *thiz, bool newTargetValue, void *ctx);

/**
 * Action: OnGetStatus
 */
typedef struct _SwitchPower_GetStatusResult
{
    bool theResultStatus;
} SwitchPower_GetStatusResult;

typedef UpnpCode (* SwitchPower_OnGetStatus)(SwitchPower *thiz, SwitchPower_GetStatusResult *result, void *ctx);

/**
 * ActionHandlers (3)
 */
typedef struct _SwitchPower_ActionHandler
{
    SwitchPower_OnGetTarget OnGetTarget;
    SwitchPower_OnSetTarget OnSetTarget;
    SwitchPower_OnGetStatus OnGetStatus;
} SwitchPower_ActionHandler;

TinyRet SwitchPower_SetHandler(SwitchPower *thiz, SwitchPower_ActionHandler *handler, void *ctx);
UpnpCode SwitchPower_OnAction(SwitchPower *thiz, UpnpAction *action);

/**
 * SendEvents
 */
TinyRet SwitchPower_SendEvents(SwitchPower *thiz);
TinyRet SwitchPower_SetStatus(SwitchPower *thiz, bool theStatus);
TinyRet SwitchPower_SetTarget(SwitchPower *thiz, bool theTarget);


TINY_END_DECLS

#endif /* __SWITCH_POWER_H__ */
