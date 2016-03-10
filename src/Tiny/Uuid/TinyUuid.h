/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   TinyUuid.h
 *
 * @remark
 *
 */

#ifndef __TINY_UUID_H__
#define __TINY_UUID_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS

#define TINY_UUID_LEN    (36 + 2)

/**************************************************************************
 *
 * uuid_t for Windows
 *
 **************************************************************************/
#ifdef _WIN32
/* uuid_t is defined in guiddef.h */
#endif /* _WIN32 */

/**************************************************************************
 *
 * uuid_t for OSX
 *
 **************************************************************************/
#ifdef __MAC_OSX__
/* uuit_t is defined in /usr/include/unistd.h */
#endif /* __MAC_OSX__ */

/**************************************************************************
 *
 * uuid_t for Linux or Android
 *
 **************************************************************************/
#if (defined __LINUX__) || (defined __ANDROID__)
typedef struct _uuid_t
{
    uint32_t    data1;          // 4
    uint16_t    data2;          // 2
    uint16_t    data3;          // 2
    uint8_t     data4[8];       // 8
} uuid_t;
#endif /* __LINUX__ || __ANDROID__ */

typedef union
{
    uuid_t      u;
    uint8_t     byte[16];
} uuid_u;

typedef struct _TinyUuid
{
    char        string[TINY_UUID_LEN];
    uuid_u      uuid;
} TinyUuid;

TinyUuid * TinyUuid_New(void);
TinyRet TinyUuid_Construct(TinyUuid *thiz);
TinyRet TinyUuid_Dispose(TinyUuid *thiz);
void TinyUuid_Delete(TinyUuid *thiz);

TinyRet TinyUuid_GenerateRandom(TinyUuid *thiz);
TinyRet TinyUuid_ParseFromString(TinyUuid *thiz, const char *string);
TinyRet TinyUuid_ParseFromBuffer(TinyUuid *thiz, const char *buffer, uint32_t len);
//uint32_t TinyUuid_ToString(TinyUuid * thiz, bool upper, char *str, uint32_t len);
const char * TinyUuid_ToString(TinyUuid * thiz, bool upper);

void TinyUuid_Clear(TinyUuid *thiz);
bool TinyUuid_Equal(TinyUuid *thiz, TinyUuid *other);
void TinyUuid_Copy(TinyUuid *dst, TinyUuid *src);
bool TinyUuid_IsNull(TinyUuid *thiz);


TINY_END_DECLS

#endif /* __TINY_UUID_H__ */
