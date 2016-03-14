/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   SoapClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __SOAP_CLIENT_H__
#define __SOAP_CLIENT_H__

#include "tiny_base.h"
#include "SoapMessage.h"

TINY_BEGIN_DECLS


struct _SoapClient;
typedef struct _SoapClient SoapClient;

SoapClient * SoapClient_New(void);
void SoapClient_Delete(SoapClient *thiz);

TinyRet SoapClient_Invoke(SoapClient *thiz, SoapMessage *request, SoapMessage *response, int timeout);


TINY_END_DECLS

#endif /* __UPNP_SOAP_CLIENT_H__ */