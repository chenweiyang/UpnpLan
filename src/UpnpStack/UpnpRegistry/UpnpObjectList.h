/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpObjectList.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_OBJECT_LIST_H__
#define __UPNP_OBJECT_LIST_H__

#include "tiny_base.h"
#include "upnp_define.h"
#include "UpnpObject.h"

TINY_BEGIN_DECLS


struct _UpnpObjectList;
typedef struct _UpnpObjectList UpnpObjectList;

UpnpObjectList * UpnpObjectList_New(void);
void UpnpObjectList_Delete(UpnpObjectList *thiz);

void UpnpObjectList_Lock(UpnpObjectList *thiz);
void UpnpObjectList_Unlock(UpnpObjectList *thiz);

void UpnpObjectList_Clear(UpnpObjectList *thiz);
UpnpObject * UpnpObjectList_GetObject(UpnpObjectList *thiz, const char *usn);
void UpnpObjectList_AddObject(UpnpObjectList *thiz, UpnpObject *object);
void UpnpObjectList_RemoveObject(UpnpObjectList *thiz, const char *usn);


TINY_END_DECLS

#endif /* __UPNP_OBJECT_LIST_H__ */