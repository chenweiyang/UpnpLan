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
#include "UpnpService.h"
#include "UpnpRuntime.h"

TINY_BEGIN_DECLS


struct _SwitchPower;
typedef struct _SwitchPower SwitchPower;

SwitchPower * SwitchPower_Create(UpnpService *service, UpnpRuntime *runtime);
void SwitchPower_Delete(SwitchPower *thiz);

/*------------------------------------------------------------------------
 * Actions (3)
 *------------------------------------------------------------------------*/

/**
* GetTarget
*/
typedef struct _SwitchPower_GetTargetResult
{
    bool theTargetValue;
} SwitchPower_GetTargetResult;

TinyRet SwitchPower_GetTarget(SwitchPower *thiz, SwitchPower_GetTargetResult *result, UpnpError *error);

/**
* SetTarget
*/
TinyRet SwitchPower_SetTarget(SwitchPower *thiz, bool newTargetValue, UpnpError *error);

/**
 * GetStatus
 */
typedef struct _SwitchPower_GetStatusResult
{
    bool theResultStatus;
} SwitchPower_GetStatusResult;

TinyRet SwitchPower_GetStatus(SwitchPower *thiz, SwitchPower_GetStatusResult *result, UpnpError *error);

/*------------------------------------------------------------------------
 * Event
 *------------------------------------------------------------------------*/
typedef void(*SwitchPower_SubscriptionExpired)(void *ctx);
typedef void(*SwitchPower_StatusChanged)(bool currentValue, void *ctx);

typedef struct _SwitchPower_EventListener
{
    SwitchPower_SubscriptionExpired     onSubscriptionExpired;
    SwitchPower_StatusChanged           onStatusChanged;
} SwitchPower_EventListener;

TinyRet SwitchPower_Subscribe(SwitchPower *thiz,
    SwitchPower_SubscriptionExpired onSubscriptionExpired,
    SwitchPower_StatusChanged onStatusChanged,
    UpnpError *error, 
    void *ctx);

TinyRet SwitchPower_Unsubscribe(SwitchPower *thiz, UpnpError *error);


TINY_END_DECLS

#endif /* __SWITCH_POWER_H__ */
