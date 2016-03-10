/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   PropertyList.h
 *
 * @remark
 *
 */

#include "PropertyList.h"
#include "TinyMap.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG             "PropertyList"


static TinyRet PropertyList_Construct(PropertyList *thiz);
static void PropertyList_Dispose(PropertyList *thiz);
static void PropertyDeleteListener(void * data, void *ctx);

struct _PropertyList
{
    TinyMap      properties;
};

PropertyList * PropertyList_New(void)
{
    PropertyList *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (PropertyList *)tiny_malloc(sizeof(PropertyList));
        if (thiz == NULL)
        {
            break;
        }

        ret = PropertyList_Construct(thiz);
        if (RET_FAILED(ret))
        {
            PropertyList_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

static TinyRet PropertyList_Construct(PropertyList *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(PropertyList));

        ret = TinyMap_Construct(&thiz->properties);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyMap_SetDeleteListener(&thiz->properties, PropertyDeleteListener, thiz);
    } while (0);

    return ret;
}

static void PropertyList_Dispose(PropertyList *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMap_Dispose(&thiz->properties);
}

void PropertyList_Delete(PropertyList * thiz)
{
    RETURN_IF_FAIL(thiz);

    PropertyList_Dispose(thiz);
    tiny_free(thiz);
}

void PropertyList_Copy(PropertyList * dst, PropertyList * src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    if (dst != src)
    {
        uint32_t i = 0;
        uint32_t count = 0;

        PropertyList_Dispose(dst);

        count = PropertyList_GetSize(src);
        for (i = 0; i < count; i++)
        {
            TinyRet ret = TINY_RET_OK;
            Property *pSrc = PropertyList_GetPropertyAt(src, i);

            Property *pDst = Property_New();
            if (pDst == NULL)
            {
                LOG_E(TAG, "Property_New failed");
                break;
            }
            Property_Copy(pDst, pSrc);

            ret = TinyMap_Insert(&dst->properties, pDst->definition.name, pDst);
            if (RET_FAILED(ret))
            {
                Property_Delete(pDst);
                LOG_E(TAG, "TinyMap_Insert failed");
                break;
            }
        }
    }
}

static void PropertyDeleteListener(void * data, void *ctx)
{
    Property *p = (Property *)data;
    tiny_free(p);
}

TinyRet PropertyList_InitProperty(PropertyList *thiz, const char *name, ObjectType *type)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        Property *property = Property_New();
        Property_Initialize(property, name, type, NULL);

        ret = TinyMap_Insert(&thiz->properties, name, property);
        if (RET_FAILED(ret))
        {
            Property_Delete(property);
        }
    } while (0);

    return ret;
}

Object * PropertyList_GetPropertyValue(PropertyList *thiz, const char *name)
{
    Object * data = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    do
    {
        Property *property = (Property *)TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            break;
        }

        data = &property->value.object;
    } while (0);

    return data;
}

TinyRet PropertyList_SetPropertyValue(PropertyList *thiz, const char *name, Object *data)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(data, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != data->type.clazzType)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_Copy(&property->value.object, data);
    } while (0);

    return ret;
}

TinyRet PropertyList_SetPropertyIntegerValue(PropertyList *thiz, const char *name, int value)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != CLAZZ_INTEGER)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_setInteger(&property->value.object, value);
    } while (0);

    return ret;
}

TinyRet PropertyList_SetPropertyLongValue(PropertyList *thiz, const char *name, long value)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != CLAZZ_LONG)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_setLong(&property->value.object, value);
    } while (0);

    return ret;
}

TinyRet PropertyList_SetPropertyFloatValue(PropertyList *thiz, const char *name, float value)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != CLAZZ_FLOAT)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_setFloat(&property->value.object, value);
    } while (0);

    return ret;
}

TinyRet PropertyList_SetPropertyDoubletValue(PropertyList *thiz, const char *name, double value)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != CLAZZ_DOUBLE)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_setDouble(&property->value.object, value);
    } while (0);

    return ret;
}

TinyRet PropertyList_SetPropertyBoolValue(PropertyList *thiz, const char *name, bool value)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != CLAZZ_BOOLEAN)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_setBool(&property->value.object, value);
    } while (0);

    return ret;
}

TinyRet PropertyList_SetPropertyCharValue(PropertyList *thiz, const char *name, char value)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != CLAZZ_CHAR)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_setChar(&property->value.object, value);
    } while (0);

    return ret;
}

TinyRet PropertyList_SetPropertyStringValue(PropertyList *thiz, const char *name, const char * value)
{
    TinyRet ret = TINY_RET_OK;
    Property *property = NULL;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    do
    {
        property = TinyMap_GetValue(&thiz->properties, name);
        if (property == NULL)
        {
            ret = TINY_RET_E_NOT_FOUND;
            break;
        }

        if (property->definition.type.clazzType != CLAZZ_STRING)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        Object_setString(&property->value.object, value);
    } while (0);

    return ret;
}

uint32_t PropertyList_GetSize(PropertyList *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyMap_GetSize(&thiz->properties);
}

Property * PropertyList_GetPropertyAt(PropertyList *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    
    return (Property *)TinyMap_GetValueAt(&thiz->properties, index);
}

Property * PropertyList_GetProperty(PropertyList *thiz, const char *name)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (Property *)TinyMap_GetValue(&thiz->properties, name);
}