/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpSsdpJob.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_SSDP_JOB_H__
#define __UPNP_SSDP_JOB_H__

#include "ct_common.h"
#include "upnp_define.h"
#include "UpnpUsn.h"

CT_BEGIN_DECLS

#define SSDP_JOB_BUF_LEN    2048

typedef struct _UpnpSsdpJob
{
    char buf[SSDP_JOB_BUF_LEN];
    char ip[CT_IP_LEN];
    uint16_t port;
    int bytes_read;
} UpnpSsdpJob;

UpnpSsdpJob * UpnpSsdpJob_New(void);
CtRet UpnpSsdpJob_Construct(UpnpSsdpJob *thiz);
CtRet UpnpSsdpJob_Dispose(UpnpSsdpJob *thiz);
void UpnpSsdpJob_Delete(UpnpSsdpJob *thiz);

const char * UpnpSsdpJob_GetBuffer(UpnpSsdpJob *thiz);
const char * UpnpSsdpJob_GetIp(UpnpSsdpJob *thiz);
uint16_t UpnpSsdpJob_GetPort(UpnpSsdpJob *thiz);
int UpnpSsdpJob_GetBytesRead(UpnpSsdpJob *thiz);


CT_END_DECLShelloworld


#endif /* __UPNP_SSDP_JOB_H__ */