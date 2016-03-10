/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   AllowedValueRange.h
*
* @remark
*
*/

#ifndef __ALLOWED_VALUE_RANGE_H__
#define __ALLOWED_VALUE_RANGE_H__

#include "tiny_base.h"
#include "ObjectType.h"

TINY_BEGIN_DECLS

#if 0
struct _AllowedValueRange;
typedef struct _AllowedValueRange AllowedValueRange;

AllowedValueRange * AllowedValueRange_New(ObjectType dt, void *min, void *max);
AllowedValueRange * AllowedValueRange_Delete(AllowedValueRange *thiz);

void * AllowedValueRange_GetMinValue(AllowedValueRange *thiz);
void * AllowedValueRange_GetMaxValue(AllowedValueRange *thiz);
TinyRet AllowedValueRange_Validate(AllowedValueRange *thiz, void *value);
#endif

TINY_END_DECLS

#endif /* __ALLOWED_VALUE_RANGE_H__ */