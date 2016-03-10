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

#include "Property.h"
#include "tiny_memory.h"

Property * Property_New()
{
    Property *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (Property *)tiny_malloc(sizeof(Property));
        if (thiz == NULL)
        {
            break;
        }

        ret = Property_Construct(thiz);
        if (RET_FAILED(ret))
        {
            Property_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

void Property_Delete(Property *thiz)
{
    RETURN_IF_FAIL(thiz);

    Property_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet Property_Construct(Property *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    PropertyDefinition_Construct(&thiz->definition);
    PropertyValue_Construct(&thiz->value);

    return TINY_RET_OK;
}

void Property_Dispose(Property *thiz)
{
    RETURN_IF_FAIL(thiz);

    PropertyDefinition_Dispose(&thiz->definition);
    PropertyValue_Dispose(&thiz->value);
}

void Property_Copy(Property *dst, Property *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        Property_Dispose(dst);

        PropertyDefinition_Copy(&dst->definition, &src->definition);
        PropertyValue_Copy(&dst->value, &src->value);
    }
}

TinyRet Property_Initialize(Property *thiz, const char *name, ObjectType *type, Object *data)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        PropertyDefinition_Initialize(&thiz->definition, name, type);

        if (data == NULL)
        {
            break;
        }

        if (data->type.clazzType != type->clazzType)
        {
            break;
        }

        PropertyValue_Initialize(&thiz->value, data);
    } while (0);

    return ret;
}