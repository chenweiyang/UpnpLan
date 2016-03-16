/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2014-9-12
 *
 * @file   TinyXmlNode.c
 *
 * @remark
 *
 */

#include "TinyXmlNode.h"
#include "TinyList.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "tiny_str_equal.h"

#define TAG                         "TinyXmlNode"
#define XML_ELEMENT_UNDEFINED       0
#define XML_ELEMENT_TAG             1
#define XML_TAG_NAME_PREFIX_LEN     128
#define XML_TAG_NAME_LEN            128

static void child_delete(void * data, void *ctx);
static void attr_delete(void * data, void *ctx);
static bool child_visit(void * data, void * ctx);

typedef struct _XmlScContent
{
    char                  * buffer;
    uint32_t                length;
} TinyXmlContent;

struct _TinyXmlNode
{
    uint32_t                type;
    char                    namePrefix[XML_TAG_NAME_PREFIX_LEN];
    char                    name[XML_TAG_NAME_LEN];
    TinyList              * attributes;
    TinyList              * children;
    TinyXmlNode           * parent;
    TinyXmlContent          content;
    uint32_t                depth;
};

TinyXmlNode * TinyXmlNode_New(void)
{
    TinyXmlNode *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyXmlNode *)tiny_malloc(sizeof(TinyXmlNode));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyXmlNode_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinyXmlNode_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet TinyXmlNode_Construct(TinyXmlNode *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinyXmlNode));
        thiz->type = XML_ELEMENT_UNDEFINED;
        thiz->depth = 0;
    } while (0);

    return ret;
}

TinyRet TinyXmlNode_Dispose(TinyXmlNode *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->type != XML_ELEMENT_UNDEFINED)
    {
        if (thiz->attributes != NULL)
        {
            TinyList_Delete(thiz->attributes);
            thiz->attributes = NULL;
        }

        if (thiz->children != NULL)
        {
            TinyList_Delete(thiz->children);
            thiz->children = NULL;
        }

        if (thiz->content.buffer != NULL)
        {
            tiny_free(thiz->content.buffer);
            thiz->content.buffer = NULL;
        }
    }

    thiz->type = XML_ELEMENT_UNDEFINED;

    return TINY_RET_OK;
}

void TinyXmlNode_Delete(TinyXmlNode *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyXmlNode_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet TinyXmlNode_AddChild(TinyXmlNode *thiz, TinyXmlNode *child)
{
    TinyRet ret = TINY_RET_OK;
    
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(child, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->children == NULL)
        {
            thiz->children = TinyList_New();
            if (thiz->children == NULL)
            {
                LOG_E(TAG, "%s", tiny_ret_to_str(TINY_RET_E_OUT_OF_MEMORY));
                ret = TINY_RET_E_OUT_OF_MEMORY;
                break;
            }

            TinyList_SetDeleteListener(thiz->children, child_delete, thiz);
        }
        
        child->depth = thiz->depth + 1;
        child->parent = thiz;
        TinyList_AddTail(thiz->children, child);
    } while (0);

    return ret;
}

TinyRet TinyXmlNode_AddAttribute(TinyXmlNode *thiz, const char *name, const char *value)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(value, TINY_RET_E_ARG_NULL);

    do
    {
        TinyXmlAttr * attr = NULL;

        if (thiz->attributes == NULL)
        {
            thiz->attributes = TinyList_New();
            if (thiz->attributes == NULL)
            {
                LOG_E(TAG, "%s", tiny_ret_to_str(TINY_RET_E_OUT_OF_MEMORY));
                ret = TINY_RET_E_OUT_OF_MEMORY;
                break;
            }

            TinyList_SetDeleteListener(thiz->attributes, attr_delete, thiz);
        }

        attr = tiny_malloc(sizeof(TinyXmlAttr));
        if (attr == NULL)
        {
            LOG_E(TAG, "%s", tiny_ret_to_str(TINY_RET_E_OUT_OF_MEMORY));
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        strncpy(attr->name, name, XML_ATTR_NAME_LEN);
        strncpy(attr->value, value, XML_ATTR_VALUE_LEN);
        TinyList_AddTail(thiz->attributes, attr);
    } while (0);

    return ret;
}

TinyRet TinyXmlNode_SetNamePrefix(TinyXmlNode *thiz, const char *namePrefix)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(namePrefix, TINY_RET_E_ARG_NULL);

    strncpy(thiz->namePrefix, namePrefix, XML_TAG_NAME_LEN);
    return TINY_RET_OK;
}

TinyRet TinyXmlNode_SetName(TinyXmlNode *thiz, const char *name)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(name, TINY_RET_E_ARG_NULL);

    thiz->type = XML_ELEMENT_TAG;
    strncpy(thiz->name, name, XML_TAG_NAME_LEN);
    return TINY_RET_OK;
}

TinyRet TinyXmlNode_SetText(TinyXmlNode *thiz, const char *data, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(data, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(len, TINY_RET_E_ARG_NULL);

    do
    {
        char *buffer = NULL;
        uint32_t buffer_size = 0;
        uint32_t old_size = 0;

        if (thiz->content.buffer != NULL)
        {
            old_size = strlen(thiz->content.buffer);
       }

        buffer_size = old_size + len + 1;
        buffer = (char *)tiny_malloc(buffer_size);
        if (buffer == NULL)
        {
            LOG_E(TAG, "%s", tiny_ret_to_str(TINY_RET_E_OUT_OF_MEMORY));
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }
        memset(buffer, 0, buffer_size);

        if (thiz->content.buffer != NULL)
        {
            strncpy(buffer, thiz->content.buffer, old_size);
        }

        strncpy(buffer + old_size, data, len);

        if (thiz->content.buffer != NULL)
        {
            tiny_free(thiz->content.buffer);
            thiz->content.buffer = NULL;
            thiz->content.length = 0;
        }

        thiz->content.buffer = buffer;
        thiz->content.length = buffer_size;

    #if TINY_XML_DEBUG 
        printf("  [%s]\n", thiz->content.buffer);
    #endif

    } while (0);

    return ret;
}

TinyXmlNode * TinyXmlNode_GetParent(TinyXmlNode *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->parent;
}

const char * TinyXmlNode_GetContent(TinyXmlNode *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
 
    return thiz->content.buffer;
}

uint32_t TinyXmlNode_GetContentLength(TinyXmlNode *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->content.length;
}

uint32_t TinyXmlNode_GetDepth(TinyXmlNode *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->depth;
}

uint32_t TinyXmlNode_GetChildren(TinyXmlNode *thiz)
{
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, 0);

    if (thiz->children != NULL)
    {
        count = TinyList_GetCount(thiz->children);
    }
    
    return count;
}

TinyXmlNode * TinyXmlNode_GetChildAt(TinyXmlNode *thiz, uint32_t index)
{
    TinyXmlNode * child = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    do
    {
        if (thiz->children == NULL)
        {
            break;
        }

        child = (TinyXmlNode *)TinyList_GetAt(thiz->children, index);
    } while (0);

    return child;
}

uint32_t TinyXmlNode_GetAttrCount(TinyXmlNode *thiz)
{
    uint32_t count = 0;

    RETURN_VAL_IF_FAIL(thiz, 0);

    if (thiz->attributes != NULL)
    {
        count = TinyList_GetCount(thiz->attributes);
    }

    return count;
}

TinyXmlAttr * TinyXmlNode_GetAttrAt(TinyXmlNode *thiz, uint32_t index)
{
    TinyXmlAttr * attr = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);

    do
    {
        if (thiz->attributes == NULL)
        {
            break;
        }

        attr = (TinyXmlAttr *)TinyList_GetAt(thiz->attributes, index);
    } while (0);

    return attr;
}

TinyXmlAttr * TinyXmlNode_GetAttr(TinyXmlNode *thiz, const char *name)
{
    TinyXmlAttr * attr = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    do
    {
        int i = 0;

        if (thiz->attributes == NULL)
        {
            break;
        }

        for (i = 0; i < TinyList_GetSize(thiz->attributes); ++i)
        {
            TinyXmlAttr *a = (TinyXmlAttr *)TinyList_GetAt(thiz->attributes, i);
            if (STR_EQUAL(a->name, name))
            {
                attr = a;
                break;
            }
        }

    } while (0);

    return attr;
}

const char * TinyXmlNode_GetNamePrefix(TinyXmlNode *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->namePrefix;
}

const char * TinyXmlNode_GetName(TinyXmlNode *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->name;
}

TinyXmlNode * TinyXmlNode_GetChildByName(TinyXmlNode *thiz, const char *name)
{
    TinyXmlNode *child = NULL;

    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    do
    {
        int index = TinyList_Foreach(thiz->children, child_visit, (void *)name);
        if (index < 0)
        {
            break;
        }

        child = (TinyXmlNode *)TinyList_FindIndex(thiz->children, index);
    } while (0);

    return child;
}

const char * TinyXmlNode_GetChildContent(TinyXmlNode *thiz, const char *name)
{
    const char *content = NULL;
    
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    do
    {
        TinyXmlNode * node = TinyXmlNode_GetChildByName(thiz, name);
        if (node == NULL)
        {
            break;
        }

        content = node->content.buffer;
    } while (0);
        
    return content;
}

static void child_delete(void * data, void *ctx)
{
    TinyXmlNode *node = (TinyXmlNode *)data;
    TinyXmlNode_Delete(node);
}

static void attr_delete(void * data, void *ctx)
{
    TinyXmlAttr * attr = (TinyXmlAttr *)data;
    tiny_free(attr);
}

static bool child_visit(void * data, void * ctx)
{
    TinyXmlNode *child = (TinyXmlNode *)data;
    const char *name = (const char *)ctx;
    if (str_equal(child->name, name, true))
    {
        return false;
    }

    return true;
}
