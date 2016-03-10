/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   SDD.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __SDD_H__
#define __SDD_H__

#include "ct_common.h"
#include "UpnpService.h"

CT_BEGIN_DECLS


CtRet SDD_Parse(const char *url, UpnpService *service, uint32_t timeout);


CT_END_DECLS

#endif /* __SDD_H__ */
