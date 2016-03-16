/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   upnp_define.h
*
* @remark
*      set tabstop4
*      set shiftwidth4
*      set expandtab
*/

#ifndef __UPNP_DEFINE_H__
#define __UPNP_DEFINE_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


#define UPNP_GROUP                              "239.255.255.250"
#define UPNP_PORT                               1900
#define UPNP_DDD_LEN                            (1024 * 20)
#define UPNP_SDD_LEN                            (1024 * 20)
#define UPNP_TIMEOUT                            (1000 * 20)
#define UPNP_SOAP_LEN                           (1024 * 20)
#define UPNP_STACK_INFO                         "UPnP/1.0 UpnpLan/1.0"
#define UPNP_STACK_INFO_LEN                     128
#define UPNP_URI_LEN                            128
#define UPNP_USN_LEN                            128
#define UPNP_UUID_LEN                           68
#define UPNP_DOMAIN_NAME_LEN                    32
#define UPNP_TYPE_LEN                           32
#define UPNP_VERSION_LEN                        32

/* Schemas */
#define SCHEMAS_UPNP_ORG                        "schemas-upnp-org"


TINY_END_DECLS

#endif /* __UPNP_DEFINE_H__ */
