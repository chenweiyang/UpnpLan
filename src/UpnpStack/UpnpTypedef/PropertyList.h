/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   PropertyList.h
 *
 * @remark
 *
 */

#ifndef __PROPERTY_LIST_H__
#define __PROPERTY_LIST_H__

#include "tiny_base.h"
#include "upnp_api.h"
#include "Property.h"

TINY_BEGIN_DECLS


struct _PropertyList;
typedef struct _PropertyList PropertyList;

UPNP_API PropertyList * PropertyList_New(void);
UPNP_API void PropertyList_Delete(PropertyList * thiz);
UPNP_API void PropertyList_Copy(PropertyList * dst, PropertyList * src);

UPNP_API TinyRet PropertyList_Add(PropertyList *thiz, const char *name, const char *value);
UPNP_API TinyRet PropertyList_AddProperty(PropertyList *thiz, Property *property);

UPNP_API uint32_t PropertyList_GetSize(PropertyList *thiz);
UPNP_API Property * PropertyList_GetPropertyAt(PropertyList *thiz, uint32_t index);
UPNP_API Property * PropertyList_GetProperty(PropertyList *thiz, const char *name);
UPNP_API const char * PropertyList_GetPropertyValue(PropertyList *thiz, const char *name);


TINY_END_DECLS

#endif /* __PROPERTY_LIST_H__ */
