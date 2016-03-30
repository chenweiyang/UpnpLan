/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpServiceParser.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_SERVICE_PARSER_H__
#define __UPNP_SERVICE_PARSER_H__

#include "tiny_base.h"
#include "UpnpService.h"

TINY_BEGIN_DECLS


TinyRet UpnpServiceParser_Parse(const char *url, UpnpService *service, uint32_t timeout);
uint32_t UpnpServiceParser_ToXml(UpnpService *service, char *xml, uint32_t len);


TINY_END_DECLS

#endif /* __UPNP_SERVICE_PARSER_H__ */
