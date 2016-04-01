/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Property.h
 *
 * @remark
 *
 */

#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include "tiny_base.h"
#include "upnp_api.h"

TINY_BEGIN_DECLS


#define PROPERTY_NAME_LEN       128
#define PROPERTY_VALUE_LEN      256


typedef struct _Property
{
    char name[PROPERTY_NAME_LEN];
    char value[PROPERTY_VALUE_LEN];
} Property;

UPNP_API Property * Property_New();
UPNP_API void Property_Delete(Property *thiz);
UPNP_API TinyRet Property_Construct(Property *thiz);
UPNP_API void Property_Dispose(Property *thiz);

UPNP_API void Property_Copy(Property *dst, Property *src);


TINY_END_DECLS

#endif /* __PROPERTY_H__ */
