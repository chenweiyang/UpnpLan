/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   ServiceSubResponse.h
*
* @remark
*
*/

#ifndef __SERVICE_SUB_RESPONSE_H__
#define __SERVICE_SUB_RESPONSE_H__

#include "tiny_base.h"
#include "UpnpSubscription.h"
#include "UpnpError.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


TinyRet ServiceSubFromResponse(UpnpSubscription *subscription, UpnpError *error, HttpMessage *response);


TINY_END_DECLS

#endif /* __SERVICE_SUB_RESPONSE_H__ */