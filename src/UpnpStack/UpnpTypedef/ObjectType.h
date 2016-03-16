/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   ObjectType.h
 *
 * @remark
 *
 */

#ifndef __OBJECT_TYPE_H__
#define __OBJECT_TYPE_H__

#include "tiny_base.h"
#include "upnp_api.h"

TINY_BEGIN_DECLS

#define CLAZZ_NAME_LEN   16

typedef enum _ClazzType
{
    CLAZZ_UNDEFINED     = 0,
    CLAZZ_BYTE          = 1,
    CLAZZ_WORD          = 2,
    CLAZZ_INTEGER       = 3,
    CLAZZ_LONG          = 4,
    CLAZZ_FLOAT         = 5,
    CLAZZ_DOUBLE        = 6,
    CLAZZ_BOOLEAN       = 7,
    CLAZZ_CHAR          = 8,
    CLAZZ_STRING        = 9,
} ClazzType;

typedef struct _ObjectType {
    ClazzType clazzType;
    char clazzName[CLAZZ_NAME_LEN];
} ObjectType;

UPNP_API void ObjectType_Construct(ObjectType *thiz);
UPNP_API void ObjectType_Dispose(ObjectType *thiz);
UPNP_API void ObjectType_Copy(ObjectType *dst, ObjectType *src);
UPNP_API void ObjectType_SetName(ObjectType *thiz, const char *name);
UPNP_API void ObjectType_SetType(ObjectType *thiz, ClazzType DataType);

UPNP_API bool ObjectType_StringToBoolean(const char *string);
UPNP_API const char * ObjectType_BooleanToString(bool b);


TINY_END_DECLS

#endif /* __OBJECT_TYPE_H__ */