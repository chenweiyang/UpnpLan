/*
 * Copyright (C) 2016-2018
 *
 * @author jxfengzi@gmail.com
 * @date   2016-3-1
 *
 * @file   upnp_macro.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __UPNP_MACRO_H__
#define __UPNP_MACRO_H__

#include <stdio.h>

#ifdef __cplusplus
    #define UPNP_BEGIN_DECLS extern "C" {
    #define UPNP_END_DECLS }
#else
    #define UPNP_BEGIN_DECLS
    #define UPNP_END_DECLS
#endif

#ifdef _WIN32
#define UPNP_INLINE              __inline
#define upnp_snprintf            _snprintf
#define upnp_strdup              _strdup
#else
#define UPNP_INLINE              inline
#define upnp_snprintf            snprintf
#define upnp_strdup              strdup
#endif /* _WIN32 */


#endif /* __UPNP_MACRO_H__ */
