/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2014-9-12
 *
 * @file   TinyXmlNode.h
 *
 * @remark
 *
 */

#ifndef __TINYXML_NODE_H__
#define __TINYXML_NODE_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS

#define TINY_XML_DEBUG                0
#define XML_ATTR_NAME_LEN           128
#define XML_ATTR_VALUE_LEN          128

typedef struct _XmlScAttr
{
    char    name[XML_ATTR_NAME_LEN];
    char    value[XML_ATTR_VALUE_LEN];
} TinyXmlAttr;

struct _TinyXmlNode;
typedef struct _TinyXmlNode TinyXmlNode;

TinyXmlNode * TinyXmlNode_New(void);
TinyRet TinyXmlNode_Construct(TinyXmlNode *thiz);
TinyRet TinyXmlNode_Dispose(TinyXmlNode *thiz);
void TinyXmlNode_Delete(TinyXmlNode *thiz);

void TinyXmlNode_SetParent(TinyXmlNode *thiz, TinyXmlNode *parent);
TinyRet TinyXmlNode_AddChild(TinyXmlNode *thiz, TinyXmlNode *child);
TinyRet TinyXmlNode_AddAttribute(TinyXmlNode *thiz, const char *name, const char *value);
TinyRet TinyXmlNode_SetName(TinyXmlNode *thiz, const char *name);
TinyRet TinyXmlNode_SetText(TinyXmlNode *thiz, const char *text, uint32_t len);

TinyXmlNode * TinyXmlNode_GetParent(TinyXmlNode *thiz);
const char * TinyXmlNode_GetName(TinyXmlNode *thiz);
const char * TinyXmlNode_GetContent(TinyXmlNode *thiz);
uint32_t TinyXmlNode_GetContentLength(TinyXmlNode *thiz);
uint32_t TinyXmlNode_GetDepth(TinyXmlNode *thiz);
uint32_t TinyXmlNode_GetChildren(TinyXmlNode *thiz);
TinyXmlNode * TinyXmlNode_GetChildAt(TinyXmlNode *thiz, uint32_t index);

uint32_t TinyXmlNode_GetAttrCount(TinyXmlNode *thiz);
TinyXmlAttr * TinyXmlNode_GetAttrAt(TinyXmlNode *thiz, uint32_t index);
TinyXmlAttr * TinyXmlNode_GetAttr(TinyXmlNode *thiz, const char *name);

TinyXmlNode * TinyXmlNode_GetChildByName(TinyXmlNode *thiz, const char *name);
const char * TinyXmlNode_GetChildContent(TinyXmlNode *thiz, const char *name);


TINY_END_DECLS

#endif /* __TINYXML_NODE_H__ */
