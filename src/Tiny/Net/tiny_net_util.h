/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   tiny_net_util.h
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_base.h"

#ifndef __TINY_NET_UTIL_H__
#define __TINY_NET_UTIL_H__

TINY_BEGIN_DECLS


typedef bool(*IpVisitor)(const char *ip, void *ctx);
TinyRet tiny_net_for_each_ip(IpVisitor visitor, void *ctx);

TinyRet tiny_net_ip_to_string(unsigned int ip, char *address, size_t len);
uint32_t tiny_net_ip_to_int(const char *ip);


TINY_END_DECLS

#endif /* __TINY_NET_UTIL_H__ */
