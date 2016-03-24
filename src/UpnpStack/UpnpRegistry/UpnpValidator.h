/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpValidator.h
*
* @remark
*
*/

#ifndef __UPNP_VALIDATOR_H__
#define __UPNP_VALIDATOR_H__

#include "tiny_base.h"
#include "UpnpUsn.h"

TINY_BEGIN_DECLS

typedef bool(*UpnpObjectFilter)(UpnpUsn *usn, void *ctx);

typedef struct _UpnpValidator
{
    bool                        strictedUuid;
    UpnpObjectFilter            filter;
    void                      * ctx;
} UpnpValidator;

TinyRet UpnpValidator_Construct(UpnpValidator *thiz);
void UpnpValidator_Dispose(UpnpValidator *thiz);

bool UpnpValidator_ValidateUSN(UpnpValidator *thiz, const char *string);
bool UpnpValidator_ValidateNT(UpnpValidator *thiz, const char *string);


TINY_END_DECLS

#endif /* __UPNP_VALIDATOR_H__ */