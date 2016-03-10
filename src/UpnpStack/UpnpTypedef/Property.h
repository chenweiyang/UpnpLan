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
#include "PropertyDefinition.h"
#include "PropertyValue.h"

TINY_BEGIN_DECLS


typedef struct _Property
{
    PropertyDefinition definition;
    PropertyValue value;
} Property;

UPNP_API Property * Property_New();
UPNP_API void Property_Delete(Property *thiz);

UPNP_API TinyRet Property_Construct(Property *thiz);
UPNP_API void Property_Dispose(Property *thiz);

UPNP_API void Property_Copy(Property *dst, Property *src);
UPNP_API TinyRet Property_Initialize(Property *thiz, const char *name, ObjectType *ObjectType, Object *Object);


TINY_END_DECLS

#endif /* __PROPERTY_H__ */
