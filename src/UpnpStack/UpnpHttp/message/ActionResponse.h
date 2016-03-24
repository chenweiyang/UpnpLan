/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   ActionResponse.h
*
* @remark
*
*/

#ifndef __ACTION_RESPONSE_H__
#define __ACTION_RESPONSE_H__

#include "tiny_base.h"
#include "UpnpAction.h"
#include "UpnpError.h"
#include "HttpMessage.h"

TINY_BEGIN_DECLS


TinyRet ActionFromResponse(UpnpAction *action, UpnpError *error, HttpMessage *response);


TINY_END_DECLS

#endif /* __ACTION_RESPONSE_H__ */