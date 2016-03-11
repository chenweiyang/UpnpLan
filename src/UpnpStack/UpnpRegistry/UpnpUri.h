/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpUri.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*
*  URI
*  =================================================================================
*  upnp:rootdevice                                         - Root Device
*  uuid:[Device UUID]                                      - Device unique identifier
*  urn:schemas-upnp-org:device:[Device Type]:[Version]     - UPnP Device Type
*  urn:schemas-upnp-org:service:[Service Type]:[Version]   - UPnP Service Type
*  urn:[Domain Name]:device:[Device Type]:[Version]        - Non-UPnP Device Type
*  urn:[Domain Name]:service:[Service Type]:[Version]      - Non-UPnP Service Type
*
*  @note
*      DLNA Architecture, Page 64
*/

#ifndef __UPNP_URI_H__
#define __UPNP_URI_H__

#include "tiny_base.h"
#include "upnp_define.h"
#include "upnp_api.h"
#include "TinyUuid.h"

TINY_BEGIN_DECLS


typedef enum _UpnpUriType
{
    UPNP_URI_UNDEFINED = 0,
    UPNP_URI_ROOT_DEVICE = 1,
    UPNP_URI_UUID = 2,
    UPNP_URI_UPNP_DEVICE = 3,
    UPNP_URI_UPNP_SERVICE = 4,
    UPNP_URI_NON_UPNP_DEVICE = 5,
    UPNP_URI_NON_UPNP_SERVICE = 6,
} UpnpUriType;

typedef struct _UpnpUri
{
    UpnpUriType type;
    char string[UPNP_URI_LEN];
    char domain_name[UPNP_DOMAIN_NAME_LEN];
    char device_type[UPNP_TYPE_LEN];
    char service_type[UPNP_TYPE_LEN];
    char version[UPNP_VERSION_LEN];
    bool strict_uuid;
    TinyUuid uuid;
    char uuid_string[UPNP_UUID_LEN];
} UpnpUri;

UPNP_API UpnpUri * UpnpUri_New(void);
UPNP_API TinyRet UpnpUri_Construct(UpnpUri *thiz);
UPNP_API TinyRet UpnpUri_Dispose(UpnpUri *thiz);
UPNP_API void UpnpUri_Delete(UpnpUri *thiz);
UPNP_API void UpnpUri_Copy(UpnpUri *dst, UpnpUri *src);

UPNP_API TinyRet UpnpUri_Initialize_RootDevice(UpnpUri *thiz);
UPNP_API TinyRet UpnpUri_Initialize_Uuid(UpnpUri *thiz, const char *uuid);
UPNP_API TinyRet UpnpUri_Initialize_UpnpDevice(UpnpUri *thiz, const char *type, const char *version);
UPNP_API TinyRet UpnpUri_Initialize_UpnpService(UpnpUri *thiz, const char *type, const char *version);
UPNP_API TinyRet UpnpUri_Initialize_NonUpnpDevice(UpnpUri *thiz, const char *domain, const char *type, const char *version);
UPNP_API TinyRet UpnpUri_Initialize_NonUpnpService(UpnpUri *thiz, const char *domain, const char *type, const char *version);

UPNP_API TinyRet UpnpUri_Parse(UpnpUri *thiz, const char *string, bool strict_uuid);
UPNP_API bool UpnpUri_IsEqual(UpnpUri *thiz, const char *string);


TINY_END_DECLS

#endif /* __UPNP_URI_H__ */