/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpInitializer.c
*
* @remark
*
*/

#include "UpnpInitializer.h"
#include "tiny_socket.h"

void UpnpInitializer_Initialize(void)
{
    tiny_socket_init();
}

void UpnpInitializer_Destroy(void)
{
    /* NOOP */
}