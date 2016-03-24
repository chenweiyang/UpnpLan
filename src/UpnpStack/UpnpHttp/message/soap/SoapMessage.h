/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   SoapMessage.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __SOAP_MESSAGE_H__
#define __SOAP_MESSAGE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "upnp_define.h"
#include "PropertyList.h"

TINY_BEGIN_DECLS


struct _SoapMessage;
typedef struct _SoapMessage SoapMessage;

UPNP_API SoapMessage * SoapMessage_New(void);
UPNP_API void SoapMessage_Delete(SoapMessage *thiz);

UPNP_API PropertyList *SoapMessage_GetArgumentList(SoapMessage *thiz);
UPNP_API PropertyList *SoapMessage_GetFault(SoapMessage *thiz);

UPNP_API TinyRet SoapMessage_Parse(SoapMessage *thiz, const char *bytes, uint32_t len);
UPNP_API TinyRet SoapMessage_ToString(SoapMessage *thiz, char *bytes, uint32_t len);

UPNP_API TinyRet SoapMessage_SetPropertyValue(SoapMessage *thiz, const char *propertyName, const char *value);
UPNP_API const char * SoapMessage_GetPropertyValue(SoapMessage *thiz, const char *propertyName);
UPNP_API TinyRet SoapMessage_SetFaultValue(SoapMessage *thiz, const char *faultName, const char *value);
UPNP_API const char * SoapMessage_GetFaultValue(SoapMessage *thiz, const char *faultName);


TINY_END_DECLS

#endif /* __SOAP_MESSAGE_H__ */