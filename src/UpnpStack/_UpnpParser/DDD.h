/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   DDD.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __DDD_H__
#define __DDD_H__

#include "ct_common.h"
#include "UpnpDevice.h"

CT_BEGIN_DECLS


CtRet DDD_Parse(const char *url, UpnpDevice *device, uint32_t timeout);


CT_END_DECLS

#endif /* __DDD_H__ */
