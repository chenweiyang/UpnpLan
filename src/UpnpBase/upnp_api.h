/*
* coding.tom@gmail.com
* 
* upnp_api.h
*
* 2011-5-14
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_API_H__
#define __UPNP_API_H__

//-----------------------------------------------------------------------------------------------
//
// UPNP_API
//
//-----------------------------------------------------------------------------------------------
#ifdef _MSC_VER
#     if (defined Upnp_shared_EXPORTS)
#		    define DLL_EXPORT _declspec(dllexport)
#	  else
#		    define DLL_EXPORT _declspec(dllimport)
#	  endif
#	
#	  define DLL_LOCAL
#
#else
#   ifdef __ANDROID__
#	  define DLL_EXPORT
#	  define DLL_LOCAL
#   else
#	  define DLL_EXPORT __attribute__ ((visibility("default")))
#	  define DLL_LOCAL __attribute__ ((visibility("hidden")))
#   endif // __ANDROID__
#endif

#define UPNP_API DLL_EXPORT


#endif // __UPNP_API_H__
