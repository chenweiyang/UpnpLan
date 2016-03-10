/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   upnp_log.h
 *
 * @remark
 *
 */

#ifndef __UPNP_LOG_H__
#define __UPNP_LOG_H__


#ifdef __ANDROID__
    #include <android/log.h>
    #define LOG_OPEN(file)
    #define LOG_CLOSE()
    #define LOG_D(tag, format, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, format, ##__VA_ARGS__)
    #define LOG_E(tag, format, ...) __android_log_print(ANDROID_LOG_ERROR, tag, format, ##__VA_ARGS__)
    #define LOG_I(tag, format, ...) __android_log_print(ANDROID_LOG_INFO, tag, format, ##__VA_ARGS__)
    #define LOG_V(tag, format, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
    #define LOG_W(tag, format, ...) __android_log_print(ANDROID_LOG_WARN, tag, format, ##__VA_ARGS__)
#else
    #include "upnp_log_print.h"
    #define LOG_OPEN(file)  __ct_log_open(file)
    #define LOG_CLOSE() __ct_log_close()

    #ifdef UPNP_DEBUG
        #define LOG_D(tag, format, ...) __upnp_log_print(UPNP_LOG_DEBUG, tag, format, ##__VA_ARGS__)
    #else
        #define LOG_D(tag, format, ...)
    #endif /* UPNP_DEBUG */

    #define LOG_E(tag, format, ...) __upnp_log_print(UPNP_LOG_ERROR, tag, format, ##__VA_ARGS__)
    #define LOG_I(tag, format, ...) __upnp_log_print(UPNP_LOG_INFO, tag, format, ##__VA_ARGS__)
    #define LOG_V(tag, format, ...) __upnp_log_print(UPNP_LOG_VERBOSE, tag, format, ##__VA_ARGS__)
    #define LOG_W(tag, format, ...) __upnp_log_print(UPNP_LOG_WARN, tag, format, ##__VA_ARGS__)
#endif /* __ANDROID__ */

#ifdef UPNP_DEBUG
    #define LOG_TIME_BEGIN(tag, func) uint64_t usec_##func = upnp_getusec()

    #ifdef __MAC_OSX__
        #define LOG_TIME_END(tag, func) LOG_D(tag, "%s, consuming: %lldms", #func, (upnp_getusec() - usec_##func) / 1000)
    #else
        #define LOG_TIME_END(tag, func) LOG_D(tag, "%s, consuming: %lums", #func, (upnp_getusec() - usec_##func) / 1000)
    #endif
#else
    #define LOG_TIME_BEGIN(tag, func)
    #define LOG_TIME_END(tag, func)
#endif

#endif /* __UPNP_LOG_H__ */