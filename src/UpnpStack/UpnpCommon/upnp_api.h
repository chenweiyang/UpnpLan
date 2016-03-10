/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   upnp_api.h
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#ifndef __UPNP_API_H__
#define __UPNP_API_H__


#ifdef _MSC_VER
#   if  (defined upnp_shared_EXPORTS)
#       define DLLEXPORT _declspec(dllexport)
#   else
#       define DLLEXPORT _declspec(dllimport)
#   endif
#   define DLLLOCAL
#else
#   ifdef __ANDROID__
#       define DLLEXPORT
#       define DLLLOCAL
#   else
#       define DLLEXPORT __attribute__ ((visibility("default")))
#       define DLLLOCAL __attribute__ ((visibility("hidden")))
#   endif /* __ANDROID__ */
#endif /* _MUPNP_VER */


#ifdef UPNP_STATIC
#   define UPNP_API
#else
#   define UPNP_API DLLEXPORT
#endif /* UPNP_STATIC */


#endif /* __UPNP_API_H__ */
