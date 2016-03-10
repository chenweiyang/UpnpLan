/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ObjectValue.h
 *
 * @remark
 *
 */

#ifndef __OBJECT_VALUE_H__
#define __OBJECT_VALUE_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


typedef union _ObjectValue
{
    int8_t byteValue;
    int16_t wordValue;
    int32_t integerValue;
    int64_t longValue;
    float floatValue;
    double doubleValue;
    char *stringValue;
    bool boolValue;
    char charValue;
    void *clazzValue;
} ObjectValue;


TINY_END_DECLS

#endif /* __OBJECT_VALUE_H__ */