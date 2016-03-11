/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   UpnpUsn.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 *
 * USN = Unique Service Name
 * =========================
 *  uuid:[Device UUID]
 *      A value for presence advertisements that convey a UUID.
 *  uuid:[Device UUID]::[root URI, device URI, or service URI]
 *      A value for presence advertisements for the root device, a device type, or a service type.
 *
 */

#ifndef __UPNP_USN_H__
#define __UPNP_USN_H__

#include "tiny_base.h"
#include "UpnpUri.h"
#include "TinyUuid.h"

TINY_BEGIN_DECLS


typedef struct _UpnpUsn
{
    char string[UPNP_USN_LEN];
    TinyUuid uuid;
    char uuid_string[UPNP_UUID_LEN];
    UpnpUri uri;
} UpnpUsn;

UpnpUsn * UpnpUsn_New(void);
TinyRet UpnpUsn_Construct(UpnpUsn *thiz);
TinyRet UpnpUsn_Dispose(UpnpUsn *thiz);
void UpnpUsn_Delete(UpnpUsn *thiz);
void UpnpUsn_Copy(UpnpUsn *dst, UpnpUsn *src);

TinyRet UpnpUsn_Initialize_Uuid(UpnpUsn *thiz, const char *uuid);
TinyRet UpnpUsn_Initialize_Uuid_UpnpUri(UpnpUsn *thiz, const char *uuid, UpnpUri *uri);

TinyRet UpnpUsn_Parse(UpnpUsn *thiz, const char *string, bool strict_uuid);


TINY_END_DECLS

#endif /* __UPNP_USN_H__ */