/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2014-9-12
 *
 * @file   TinyXml.h
 *
 * @remark
 *
 */

#ifndef __TINY_XML_H__
#define __TINY_XML_H__

#include "tiny_base.h"
#include "TinyXmlNode.h"

TINY_BEGIN_DECLS


struct _TinyXml;
typedef struct _TinyXml TinyXml;

TinyXml * TinyXml_New(void);
TinyRet TinyXml_Construct(TinyXml *thiz);
TinyRet TinyXml_Dispose(TinyXml *thiz);
void TinyXml_Delete(TinyXml *thiz);

TinyRet TinyXml_Load(TinyXml *thiz, const char *file);
TinyRet TinyXml_Parse(TinyXml *thiz, const char *buffer, uint32_t length);
TinyRet TinyXml_ToString(TinyXml *thiz, char **string, uint32_t *length);
TinyXmlNode * TinyXml_GetRoot(TinyXml *thiz);


TINY_END_DECLS

#endif /* __TINY_XML_H__ */
