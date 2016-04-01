/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   InternalData.h
*
* @remark
*
*/

#ifndef __INTERNAL_DATA_H__
#define __INTERNAL_DATA_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


typedef enum _InternalType
{
    INTERNAL_UNDEFINED = 0,
    INTERNAL_BYTE = 1,
    INTERNAL_WORD = 2,
    INTERNAL_INTEGER = 3,
    INTERNAL_LONG = 4,
    INTERNAL_FLOAT = 5,
    INTERNAL_DOUBLE = 6,
    INTERNAL_BOOLEAN = 7,
    INTERNAL_CHAR = 8,
    INTERNAL_STRING = 9,
} InternalType;

typedef union _InternalValue
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
} InternalValue;


TINY_END_DECLS

#endif /* __INTERNAL_DATA_H__ */