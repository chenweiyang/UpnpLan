/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   Object.h
 *
 * @remark
 *
 */

#ifndef __Object_H__
#define __Object_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "ObjectType.h"
#include "ObjectValue.h"

TINY_BEGIN_DECLS


typedef struct _Object
{
    ObjectType type;
    ObjectValue value;
} Object;

UPNP_API void Object_Construct(Object *thiz);
UPNP_API void Object_Dispose(Object *thiz);
UPNP_API void Object_Copy(Object *dst, Object *src);

UPNP_API TinyRet Object_setValue(Object *thiz, const char *value);

UPNP_API void Object_setByte(Object *thiz, int8_t value);
UPNP_API void Object_setWord(Object *thiz, int16_t value);
UPNP_API void Object_setInteger(Object *thiz, int32_t value);
UPNP_API void Object_setLong(Object *thiz, int64_t value);
UPNP_API void Object_setFloat(Object *thiz, float value);
UPNP_API void Object_setDouble(Object *thiz, double value);
UPNP_API void Object_setBool(Object *thiz, bool value);
UPNP_API void Object_setChar(Object *thiz, char value);
UPNP_API void Object_setString(Object *thiz, const char * value);


TINY_END_DECLS

#endif /* __Object_H__ */