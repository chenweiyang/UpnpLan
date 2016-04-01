/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpArgumentDirection.h
*
* @remark
*
*/

#ifndef __UPNP_ARGUMENT_DIRECTION_H__
#define __UPNP_ARGUMENT_DIRECTION_H__

#include "tiny_base.h"
#include "upnp_api.h"

TINY_BEGIN_DECLS


typedef enum _UpnpArgumentDirection
{
    ARG_UNKNOWN = -1,
    ARG_IN = 0,
    ARG_OUT = 1,
} UpnpArgumentDirection;

UPNP_API UpnpArgumentDirection UpnpArgumentDirection_Retrieve(const char *string);
UPNP_API const char * UpnpArgumentDirection_ToString(UpnpArgumentDirection direction);


TINY_END_DECLS

#endif /* __UPNP_ARGUMENT_DIRECTION_H__ */