/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   PropertyDefinition.h
 *
 * @remark
 *
 */

#ifndef __PROPERTY_DEFINITION_H__
#define __PROPERTY_DEFINITION_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "ObjectType.h"

TINY_BEGIN_DECLS


#define PROPERTY_NAME_LEN   64

typedef struct _PropertyDefinition
{
    char name[PROPERTY_NAME_LEN];
    ObjectType type;
} PropertyDefinition;

UPNP_API void PropertyDefinition_Construct(PropertyDefinition *thiz);
UPNP_API void PropertyDefinition_Dispose(PropertyDefinition *thiz);
UPNP_API void PropertyDefinition_Copy(PropertyDefinition *dst, PropertyDefinition *src);
UPNP_API void PropertyDefinition_Initialize(PropertyDefinition *thiz, const char *name, ObjectType *type);


TINY_END_DECLS

#endif /* __PROPERTY_DEFINITION_H__ */