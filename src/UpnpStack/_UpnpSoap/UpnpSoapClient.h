/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSoapClient.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_SOAP_CLIENT_H__
#define __UPNP_SOAP_CLIENT_H__

#include "ct_common.h"
#include "UpnpSoapMessage.h"

CT_BEGIN_DECLS


struct _UpnpSoapClient;
typedef struct _UpnpSoapClient UpnpSoapClient;

UpnpSoapClient * UpnpSoapClient_New(void);
void UpnpSoapClient_Delete(UpnpSoapClient *thiz);

CtRet UpnpSoapClient_Invoke(UpnpSoapClient *thiz,
    UpnpSoapMessage *request,
    UpnpSoapMessage *response,
    int timeout);


CT_END_DECLS

#endif /* __UPNP_SOAP_CLIENT_H__ */