/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   DataValue.h
*
* @remark
*
*/

#ifndef __DATA_VALUE_H__
#define __DATA_VALUE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "InternalData.h"

TINY_BEGIN_DECLS


typedef struct _DataValue
{
    InternalType internalType;
    InternalValue internalValue;
} DataValue;

UPNP_API void DataValue_Construct(DataValue *thiz);
UPNP_API void DataValue_Dispose(DataValue *thiz);
UPNP_API void DataValue_Copy(DataValue *dst, DataValue *src);

UPNP_API TinyRet DataValue_GetValue(DataValue *thiz, char *value, uint32_t len);
UPNP_API TinyRet DataValue_SetValue(DataValue *thiz, const char *value);

UPNP_API void DataValue_SetByte(DataValue *thiz, int8_t value);
UPNP_API void DataValue_SetWord(DataValue *thiz, int16_t value);
UPNP_API void DataValue_SetInteger(DataValue *thiz, int32_t value);
UPNP_API void DataValue_SetLong(DataValue *thiz, int64_t value);
UPNP_API void DataValue_SetFloat(DataValue *thiz, float value);
UPNP_API void DataValue_SetDouble(DataValue *thiz, double value);
UPNP_API void DataValue_SetBool(DataValue *thiz, bool value);
UPNP_API void DataValue_SetChar(DataValue *thiz, char value);
UPNP_API void DataValue_SetString(DataValue *thiz, const char * value);


TINY_END_DECLS

#endif /* __DATA_VALUE_H__ */