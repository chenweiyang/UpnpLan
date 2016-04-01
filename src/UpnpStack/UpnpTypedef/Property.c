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

TinyRet Property_Construct(Property *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz->name, 0, PROPERTY_NAME_LEN);
    memset(thiz->value, 0, PROPERTY_VALUE_LEN);

    return TINY_RET_OK;
}

void Property_Dispose(Property *thiz)
{
    RETURN_IF_FAIL(thiz);

    memset(thiz, 0, sizeof(Property));
}

void Property_Delete(Property *thiz)
{
    RETURN_IF_FAIL(thiz);

    Property_Dispose(thiz);
    tiny_free(thiz);
}

void Property_Copy(Property *dst, Property *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        Property_Dispose(dst);

        strncpy(dst->name, src->name, PROPERTY_NAME_LEN);
        strncpy(dst->value, src->value, PROPERTY_VALUE_LEN);
    }
}