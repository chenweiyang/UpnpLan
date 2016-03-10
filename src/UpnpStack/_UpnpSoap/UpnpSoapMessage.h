/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSoapMessage.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_SOAP_MESSAGE_H__
#define __UPNP_SOAP_MESSAGE_H__

#include "ct_common.h"
#include "upnp_api.h"
#include "upnp_define.h"
#include "PropertyList.h"

CT_BEGIN_DECLS


struct _UpnpSoapMessage;
typedef struct _UpnpSoapMessage UpnpSoapMessage;

UPNP_API UpnpSoapMessage * UpnpSoapMessage_New(void);
UPNP_API void UpnpSoapMessage_Delete(UpnpSoapMessage *thiz);

UPNP_API PropertyList *UpnpSoapMessage_GetArgumentList(UpnpSoapMessage *thiz);
UPNP_API PropertyList *UpnpSoapMessage_GetFault(UpnpSoapMessage *thiz);

UPNP_API CtRet UpnpSoapMessage_Parse(UpnpSoapMessage *thiz, const char *bytes, uint32_t len);
UPNP_API CtRet UpnpSoapMessage_ToString(UpnpSoapMessage *thiz, char *bytes, uint32_t len);

UPNP_API CtRet UpnpSoapMessage_SetPropertyValue(UpnpSoapMessage *thiz, const char *propertyName, const char *value);
UPNP_API const char * UpnpSoapMessage_GetPropertyValue(UpnpSoapMessage *thiz, const char *propertyName);
UPNP_API CtRet UpnpSoapMessage_SetFaultValue(UpnpSoapMessage *thiz, const char *faultName, const char *value);
UPNP_API const char * UpnpSoapMessage_GetFaultValue(UpnpSoapMessage *thiz, const char *faultName);


CT_END_DECLS

#endif /* __SOAP_MESSAGE_H__ */