/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   EventRequest.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __EVENT_REQUEST__
#define __EVENT_REQUEST__

#include "tiny_base.h"
#include "UpnpEvent.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


TinyRet UpnpEventToRequest(UpnpEvent *action, HttpMessage *request);


TINY_END_DECLS

#endif /* __EVENT_REQUEST__ */
