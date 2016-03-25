/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   tiny_net_util.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "tiny_net_util.h"
#include "tiny_log.h"
#include "tiny_str_split.h"
#include <stdlib.h>

#define TAG     "tiny_net_util"

#ifdef _WIN32
#include <Iphlpapi.h>
#pragma comment(lib, "Iphlpapi.lib") 

TinyRet tiny_net_for_each_ip(IpVisitor visitor, void *ctx)
{
    TinyRet ret = TINY_RET_E_NOT_FOUND;

    RETURN_VAL_IF_FAIL(visitor, TINY_RET_E_ARG_NULL);

    IP_ADAPTER_INFO * pInfo = NULL;
    IP_ADAPTER_INFO * pInfoHead = NULL;
    ULONG ulOutBufLen = 0;
    DWORD dwRetVal = 0;

    do
    {
        pInfo = (IP_ADAPTER_INFO *)GlobalAlloc(GPTR, sizeof(IP_ADAPTER_INFO));
        ulOutBufLen = sizeof(IP_ADAPTER_INFO);

        if (ERROR_BUFFER_OVERFLOW == GetAdaptersInfo(pInfo, &ulOutBufLen))
        {
            GlobalFree(pInfo);
            pInfo = (IP_ADAPTER_INFO *)GlobalAlloc(GPTR, ulOutBufLen);
        }

        if (pInfo == NULL)
        {
            break;
        }

        if (dwRetVal = GetAdaptersInfo(pInfo, &ulOutBufLen))
        {
            LOG_E(TAG, "Call to GetAdaptersInfo failed. Return Value: %08x\r\n", dwRetVal);
            GlobalFree(pInfo);
            break;
        }

        pInfoHead = pInfo;

        do
        {
            PIP_ADDR_STRING p = &pInfoHead->IpAddressList;

            while (p != NULL)
            {
                if (visitor(p->IpAddress.String, ctx))
                {
                    ret = TINY_RET_OK;
                    break;
                }

                p = p->Next;
            }

            if (RET_SUCCEEDED(ret))
            {
                break;
            }

            pInfo = pInfo->Next;
        } while (pInfo);
    
        GlobalFree(pInfoHead);
    } while (0);

    return ret;
}

#else

#include <sys/types.h>  
#include <ifaddrs.h>  
#include <netinet/in.h>  
#include <string.h>  
#include <arpa/inet.h>  

TinyRet tiny_net_for_each_ip(IpVisitor visitor, void *ctx)
{
    TinyRet ret = TINY_RET_E_NOT_FOUND;

    RETURN_VAL_IF_FAIL(visitor, TINY_RET_E_ARG_NULL);

    do
    {
        struct ifaddrs* ifa = NULL, *oifa;
        if (getifaddrs(&ifa) < 0)
        {
            break;
        }

        oifa = ifa;
        while (ifa != NULL)
        {
            // IPv4 ÅÅ³ýlocalhost  
            if (ifa->ifa_addr != NULL && ifa->ifa_addr->sa_family == AF_INET && strncmp(ifa->ifa_name, "lo", 2) != 0)
            {
                struct sockaddr_in* saddr = (struct sockaddr_in*)ifa->ifa_addr;
                char ip[32];
                memset(ip, 0, 32);
                tiny_net_ip_to_string(ntohl(saddr->sin_addr.s_addr), ip, 32);

                if (visitor(ip, ctx))
                {
                    ret = TINY_RET_OK;
                    break;
                }
            }

            ifa = ifa->ifa_next;
        }

        freeifaddrs(oifa);
    } while (0);

    return ret;
}
#endif

TinyRet tiny_net_ip_to_string(unsigned int ip, char address[32], size_t len)
{
    RETURN_VAL_IF_FAIL(address, 0);

    tiny_snprintf(address, len, "%d.%d.%d.%d",
        (ip & 0xFF),
        ((ip >> 8) & 0xFF),
        ((ip >> 16) & 0xFF),
        ((ip >> 24) & 0xFF));

    return TINY_RET_OK;
}

uint32_t tiny_net_ip_to_int(const char *ip)
{
    uint32_t ret = 0;
    char group[4][128];
    const char *dot = ".";

    RETURN_VAL_IF_FAIL(ip, 0);

    do
    {
        uint32_t a[4];

        memset(group, 0, 4 * 128);
        ret = str_split(ip, dot, group, 4);
        if (ret != 4)
        {
            ret = 0;
            break;
        }

        a[0] = atoi(group[0]);
        a[1] = atoi(group[1]);
        a[2] = atoi(group[2]);
        a[3] = atoi(group[3]);

        ret = (a[0] & 0x000000FF)
            + ((a[1] << 8) & 0x0000FF00)
            + ((a[2] << 16) & 0x00FF0000)
            + ((a[3] << 24) & 0xFF000000);
    } while (0);

    return ret;
}
