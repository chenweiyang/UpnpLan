/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   PropertyDefinition
 *
 * @remark
 *
 */

#include "PropertyDefinition.h"

#define TAG     "PropertyDefinition"

void PropertyDefinition_Construct(PropertyDefinition *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(PropertyDefinition));
}

void PropertyDefinition_Dispose(PropertyDefinition *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(PropertyDefinition));
}

void PropertyDefinition_Copy(PropertyDefinition *dst, PropertyDefinition *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        PropertyDefinition_Dispose(dst);

        ObjectType_Copy(&dst->type, &src->type);
        strncpy(dst->name, src->name, PROPERTY_NAME_LEN);
    }
}

void PropertyDefinition_Initialize(PropertyDefinition *thiz, const char *name, ObjectType *type)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);

    ObjectType_Copy(&thiz->type, type);
    strncpy(thiz->name, name, PROPERTY_NAME_LEN);
}