/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   ServiceSubRequest.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __SERVICE_SUB_REQUEST_H__
#define __SERVICE_SUB_REQUEST_H__

#include "tiny_base.h"
#include "UpnpSubscription.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


TinyRet ServiceSubToRequest(UpnpSubscription *subscription, HttpMessage *request);


TINY_END_DECLS

#endif /* __SERVICE_SUB_REQUEST_H__ */