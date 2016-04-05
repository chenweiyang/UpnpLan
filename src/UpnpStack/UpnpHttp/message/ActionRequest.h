/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   ActionRequest.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __ACTION_REQUEST__
#define __ACTION_REQUEST__

#include "tiny_base.h"
#include "UpnpAction.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


TinyRet ActionToRequest(UpnpAction *action, HttpMessage *request);
TinyRet ActionFromRequest(UpnpAction *action, const char *content, uint32_t contentLength);


TINY_END_DECLS

#endif /* __ACTION_REQUEST__ */
