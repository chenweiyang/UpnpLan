/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   upnp_net_util.h
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_base.h"

#ifndef __UPNP_NET_UTIL_H__
#define __UPNP_NET_UTIL_H__

TINY_BEGIN_DECLS


TinyRet upnp_net_get_ip(const char *remoteIp, char *selfIp, size_t len);


TINY_END_DECLS

#endif /* __UPNP_NET_UTIL_H__ */
