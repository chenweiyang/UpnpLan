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
#include "TinyList.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG             "PropertyList"


static TinyRet PropertyList_Construct(PropertyList *thiz);
static void PropertyList_Dispose(PropertyList *thiz);
static void PropertyDeleteListener(void * data, void *ctx);

struct _PropertyList
{
    TinyList      properties;
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

        ret = TinyList_Construct(&thiz->properties);
        if (RET_FAILED(ret))
        {
            break;
        }

        TinyList_SetDeleteListener(&thiz->properties, PropertyDeleteListener, thiz);
    } while (0);

    return ret;
}

static void PropertyList_Dispose(PropertyList *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyList_Dispose(&thiz->properties);
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
        uint32_t count = PropertyList_GetSize(src);

        PropertyList_Dispose(dst);

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

            ret = TinyList_AddTail(&dst->properties, pDst);
            if (RET_FAILED(ret))
            {
                Property_Delete(pDst);
                LOG_E(TAG, "TinyList_AddTail failed");
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

TinyRet PropertyList_Add(PropertyList *thiz, const char *name, const char *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        Property *p = Property_New();
        if (p == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        strncpy(p->name, name, PROPERTY_NAME_LEN);
        strncpy(p->value, value, PROPERTY_VALUE_LEN);

        ret = PropertyList_AddProperty(thiz, p);
        if (RET_FAILED(ret))
        {
            Property_Delete(p);
            break;
        }
    } while (0);

    return ret;
}

TinyRet PropertyList_AddProperty(PropertyList *thiz, Property *property)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(property, TINY_RET_E_ARG_NULL);

    do
    {
        bool found = false;
        uint32_t i = 0;
        uint32_t count = 0;

        count = TinyList_GetCount(&thiz->properties);

        for (i = 0; i < count; ++i)
        {
            Property *p = TinyList_GetAt(&thiz->properties, i);
            if (STR_EQUAL(p->name, property->name))
            {
                found = true;
                break;
            }
        }

        if (found)
        {
            ret = TINY_RET_E_ITEM_EXIST;
            break;
        }

        ret = TinyList_AddTail(&thiz->properties, property);
    } while (0);

    return ret;
}

uint32_t PropertyList_GetSize(PropertyList *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyList_GetSize(&thiz->properties);
}

Property * PropertyList_GetPropertyAt(PropertyList *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    
    return (Property *)TinyList_GetAt(&thiz->properties, index);
}

Property * PropertyList_GetProperty(PropertyList *thiz, const char *name)
{
    uint32_t i = 0;
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    count = TinyList_GetCount(&thiz->properties);

    for (i = 0; i < count; ++i)
    {
        Property *p = TinyList_GetAt(&thiz->properties, i);
        if (STR_EQUAL(p->name, name))
        {
            return p;
        }
    }

    return NULL;
}

const char * PropertyList_GetPropertyValue(PropertyList *thiz, const char *name)
{
    Property * p = PropertyList_GetProperty(thiz, name);
    if (p != NULL)
    {
        return p->value;
    }

    return NULL;
}