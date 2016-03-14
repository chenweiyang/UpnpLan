/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpError.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_ERROR_H__
#define __UPNP_ERROR_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS

#define UPNP_ERR_DESCRIPTION_LEN                256

typedef struct _UpnpError
{
    int code;
    char description[UPNP_ERR_DESCRIPTION_LEN];
} UpnpError;


TINY_END_DECLS

#endif /* __UPNP_ERROR_H__ */
