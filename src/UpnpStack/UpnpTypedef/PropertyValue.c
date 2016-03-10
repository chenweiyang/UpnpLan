/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   PropertyValue
 *
 * @remark
 *
 */

#include "PropertyValue.h"

#define TAG     "PropertyValue"

void PropertyValue_Construct(PropertyValue *thiz)
{
    RETURN_IF_FAIL(thiz);

    thiz->isChanged = false;
    Object_Construct(&thiz->object);
}

void PropertyValue_Dispose(PropertyValue *thiz)
{
    RETURN_IF_FAIL(thiz);

    thiz->isChanged = false;
    Object_Dispose(&thiz->object);
}

void PropertyValue_Copy(PropertyValue *dst, PropertyValue *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        PropertyValue_Dispose(dst);

        dst->isChanged = src->isChanged;
        Object_Copy(&dst->object, &src->object);
    }
}

void PropertyValue_Initialize(PropertyValue *thiz, Object *object)
{
    RETURN_IF_FAIL(thiz);

    Object_Copy(&thiz->object, object);
}