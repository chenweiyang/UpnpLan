/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   upnp_timeout_util.h
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_base.h"

#ifndef __UPNP_TIMEOUT_UTIL_H__
#define __UPNP_TIMEOUT_UTIL_H__

TINY_BEGIN_DECLS


TinyRet upnp_timeout_get_second(const char *timeout, uint32_t *second);
TinyRet upnp_timeout_to_string(uint32_t second, char *string, uint32_t len);


TINY_END_DECLS

#endif /* __UPNP_TIMEOUT_UTIL_H__ */
