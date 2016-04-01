/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   DataType.h
*
* @remark
*
*/

#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "InternalData.h"

TINY_BEGIN_DECLS


#define DATA_TYPE_NAME_LEN   16

typedef struct _DataType
{
    InternalType internalType;
    char name[DATA_TYPE_NAME_LEN];
} DataType;

UPNP_API void DataType_Construct(DataType *thiz);
UPNP_API void DataType_Dispose(DataType *thiz);
UPNP_API void DataType_Copy(DataType *dst, DataType *src);
UPNP_API void DataType_SetName(DataType *thiz, const char *name);
UPNP_API void DataType_SetType(DataType *thiz, InternalType type);

UPNP_API bool DataType_StringToBoolean(const char *string);
UPNP_API const char * DataType_BooleanToString(bool b);


TINY_END_DECLS

#endif /* __DATA_TYPE_H__ */