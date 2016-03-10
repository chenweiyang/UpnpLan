/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   AllowedValueList.h
*
* @remark
*
*/

#ifndef __ALLOWED_VALUE_LIST_H__
#define __ALLOWED_VALUE_LIST_H__

#include "tiny_base.h"
#include "ObjectType.h"

TINY_BEGIN_DECLS

#if 0
struct _AllowedValueList;
typedef struct _AllowedValueList AllowedValueList;

AllowedValueList * AllowedValueList_New(ObjectType dt);
AllowedValueList * AllowedValueList_Delete(AllowedValueList *thiz);

TinyRet AllowedValueList_AddValue(AllowedValueList *thiz, void *value);
TinyRet AllowedValueList_Validate(AllowedValueList *thiz, void *value);
#endif

TINY_END_DECLS

#endif /* __ALLOWED_VALUE_LIST_H__ */