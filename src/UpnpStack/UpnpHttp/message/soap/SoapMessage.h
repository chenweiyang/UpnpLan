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
#include "upnp_define.h"
#include "PropertyList.h"

TINY_BEGIN_DECLS


struct _SoapMessage;
typedef struct _SoapMessage SoapMessage;

SoapMessage * SoapMessage_New(void);
void SoapMessage_Delete(SoapMessage *thiz);

PropertyList *SoapMessage_GetArgumentList(SoapMessage *thiz);
PropertyList *SoapMessage_GetFault(SoapMessage *thiz);

TinyRet SoapMessage_ParseRequest(SoapMessage *thiz, const char *bytes, uint32_t len);
TinyRet SoapMessage_ParseResponse(SoapMessage *thiz, const char *bytes, uint32_t len);
TinyRet SoapMessage_ToString(SoapMessage *thiz, char *bytes, uint32_t len);

TinyRet SoapMessage_SetServerURL(SoapMessage *thiz, const char *serverURL);
TinyRet SoapMessage_SetActionName(SoapMessage *thiz, const char *actionName);
TinyRet SoapMessage_SetResponseActionName(SoapMessage *thiz, const char *actionName);
TinyRet SoapMessage_SetActionXmlns(SoapMessage *thiz, const char *actionXmlns);
const char * SoapMessage_GetServerURL(SoapMessage *thiz);
const char * SoapMessage_GetActionName(SoapMessage *thiz);
const char * SoapMessage_GetActionXmlns(SoapMessage *thiz);

bool SoapMessage_IsFault(SoapMessage *thiz);
TinyRet SoapMessage_SetFault(SoapMessage *thiz, int faultCode, const char *faultstring);
TinyRet SoapMessage_SetError(SoapMessage *thiz, int errorCode, const char *errorDescription);
int SoapMessage_GetFaultCode(SoapMessage *thiz);
const char * SoapMessage_GetFaultstring(SoapMessage *thiz);
int SoapMessage_GetErrorCode(SoapMessage *thiz);
const char * SoapMessage_GetErrorDescription(SoapMessage *thiz);



TINY_END_DECLS

#endif /* __SOAP_MESSAGE_H__ */