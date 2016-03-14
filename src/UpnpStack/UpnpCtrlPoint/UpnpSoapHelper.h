/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpSoapHelper.h
*
* @remark
*
*/

#ifndef __UPNP_SOAP_HELPER_H__
#define __UPNP_SOAP_HELPER_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "SoapMessage.h"
#include "UpnpAction.h"
#include "UpnpError.h"

TINY_BEGIN_DECLS


TinyRet SoapMessageToActionResult(SoapMessage *soap, UpnpAction *action, UpnpError *error);
TinyRet ActionToSoapMessage(UpnpAction *action, SoapMessage *soap);


TINY_END_DECLS

#endif /* __UPNP_SOAP_HELPER_H__ */