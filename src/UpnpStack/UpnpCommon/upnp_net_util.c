/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   upnp_net_util.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "upnp_net_util.h"
#include "tiny_net_util.h"
#include "tiny_log.h"

#define TAG     "upnp_net_util"

typedef struct _VisitorContext
{
    const char *remoteIp;
    char *selfIp;
    size_t len;
} VisitorContext;

static bool visitor(const char *ip, void *ctx)
{
    VisitorContext * c = (VisitorContext *) ctx;
    uint32_t remoteIp = tiny_net_ip_to_int(c->remoteIp);;
    uint32_t selfIp = tiny_net_ip_to_int(ip);;

    if ((remoteIp & 0x00FFFFFF) == (selfIp & 0x00FFFFFF))
    {
        return true;
    }

    return false;
}

TinyRet upnp_net_get_ip(const char *remoteIp, char *selfIp, size_t len)
{
    VisitorContext c;

    c.remoteIp = remoteIp;
    c.selfIp = selfIp;
    c.len = len;

    return tiny_net_for_each_ip(visitor, &c);
}