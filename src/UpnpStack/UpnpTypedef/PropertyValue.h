/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   PropertyValue.h
 *
 * @remark
 *
 */

#ifndef __PROPERTY_VALUE_H__
#define __PROPERTY_VALUE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "Object.h"

TINY_BEGIN_DECLS


typedef struct _PropertyValue
{
    bool isChanged;
    Object object;
} PropertyValue;

UPNP_API void PropertyValue_Construct(PropertyValue *thiz);
UPNP_API void PropertyValue_Dispose(PropertyValue *thiz);
UPNP_API void PropertyValue_Copy(PropertyValue *dst, PropertyValue *src);
UPNP_API void PropertyValue_Initialize(PropertyValue *thiz, Object *object);


TINY_END_DECLS

#endif /* __PROPERTY_VALUE_H__ */
