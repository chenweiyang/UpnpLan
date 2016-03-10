/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   DataValue
 *
 * @remark
 *
 */

#include "DataValue.h"

#define TAG     "DataValue"

void DataValue_Copy(DataValue *dst, DataValue *src)
{
    memset(dst, src, sizeof(DataValue));
}