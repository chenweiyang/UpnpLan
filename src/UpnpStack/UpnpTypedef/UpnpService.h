/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpService.h
*
* @remark
*
*/

#ifndef __UPNP_SERVICE_H__
#define __UPNP_SERVICE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "UpnpAction.h"
#include "UpnpStateVariable.h"
#include "UpnpSubscriber.h"

TINY_BEGIN_DECLS


struct _UpnpService;
typedef struct _UpnpService UpnpService;

UPNP_API UpnpService * UpnpService_New(void);
UPNP_API void UpnpService_Delete(UpnpService *thiz);

UPNP_API void UpnpService_SetParentDevice(UpnpService *thiz, void *device);
UPNP_API void * UpnpService_GetParentDevice(UpnpService *thiz);

typedef void(*UpnpServiceChangedListener)(UpnpService *service, void *ctx);
UPNP_API void UpnpService_SetChangedListener(UpnpService *thiz, UpnpServiceChangedListener listener, void *ctx);
UPNP_API TinyRet UpnpService_SendEvents(UpnpService *thiz);

UPNP_API TinyRet UpnpService_SetServiceType(UpnpService *thiz, const char *serviceType);
UPNP_API TinyRet UpnpService_SetServiceId(UpnpService *thiz, const char *serviceId);
UPNP_API TinyRet UpnpService_SetControlURL(UpnpService *thiz, const char *controlURL);
UPNP_API TinyRet UpnpService_SetEventSubURL(UpnpService *thiz, const char *eventSubURL);
UPNP_API TinyRet UpnpService_SetSCPDURL(UpnpService *thiz, const char *SCPDURL);
UPNP_API TinyRet UpnpService_SetCallbackURI(UpnpService *thiz, const char *callbackURI);

UPNP_API const char * UpnpService_GetServiceType(UpnpService *thiz);
UPNP_API const char * UpnpService_GetServiceId(UpnpService *thiz);
UPNP_API const char * UpnpService_GetControlURL(UpnpService *thiz);
UPNP_API const char * UpnpService_GetEventSubURL(UpnpService *thiz);
UPNP_API const char * UpnpService_GetSCPDURL(UpnpService *thiz);
UPNP_API const char * UpnpService_GetCallbackURI(UpnpService *thiz);

UPNP_API TinyRet UpnpService_AddAction(UpnpService *thiz, UpnpAction *action);
UPNP_API uint32_t UpnpService_GetActionCount(UpnpService *thiz);
UPNP_API UpnpAction * UpnpService_GetActionAt(UpnpService *thiz, uint32_t index);
UPNP_API UpnpAction * UpnpService_GetAction(UpnpService *thiz, const char *actionName);

UPNP_API TinyRet UpnpService_AddStateVariable(UpnpService *thiz, UpnpStateVariable *stateVariable);
UPNP_API uint32_t UpnpService_GetStateVariableCount(UpnpService *thiz);
UPNP_API UpnpStateVariable * UpnpService_GetStateVariableAt(UpnpService *thiz, uint32_t index);
UPNP_API UpnpStateVariable * UpnpService_GetStateVariable(UpnpService *thiz, const char *stateName);

UPNP_API TinyRet UpnpService_AddSubscriber(UpnpService *thiz, UpnpSubscriber *subscriber);
UPNP_API TinyRet UpnpService_RemoveSubscriber(UpnpService *thiz, const char *sid);
UPNP_API uint32_t UpnpService_GetSubscriberCount(UpnpService *thiz);
UPNP_API UpnpSubscriber * UpnpService_GetSubscriberAt(UpnpService *thiz, uint32_t index);
UPNP_API UpnpSubscriber * UpnpService_GetSubscriber(UpnpService *thiz, const char *callback);



TINY_END_DECLS

#endif /* __UPNP_SERVICE_H__ */