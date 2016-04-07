/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   TinyMulticastSocket.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "TinyMulticastSocket.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "tiny_net_util.h"
#include "tiny_socket.h"

#define TAG         "TinyMulticastSocket"

TinyMulticastSocket * TinyMulticastSocket_New(void)
{
    TinyMulticastSocket *thiz = NULL;

    do
    {
        thiz = (TinyMulticastSocket *)tiny_malloc(sizeof(TinyMulticastSocket));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(TinyMulticastSocket_Construct(thiz)))
        {
            TinyMulticastSocket_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet TinyMulticastSocket_Construct(TinyMulticastSocket *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinyMulticastSocket));
        thiz->fd = 0;
    } while (0);

    return TINY_RET_OK;
}

void TinyMulticastSocket_Dispose(TinyMulticastSocket *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMulticastSocket_Close(thiz);
    memset(thiz, 0, sizeof(TinyMulticastSocket));
}

void TinyMulticastSocket_Delete(TinyMulticastSocket *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMulticastSocket_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet TinyMulticastSocket_Open(TinyMulticastSocket *thiz, unsigned long ip, const char *group, uint16_t port, bool block)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(group, TINY_RET_E_ARG_NULL);

    do
    {
        ret = tiny_net_ip_to_string(ip, thiz->ip, TINY_IP_LEN);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "tiny_net_ip_to_string failed");
            break;
        }

        thiz->fd = socket(AF_INET, SOCK_DGRAM, 0);
        if (thiz->fd < 0)
        {
            LOG_E(TAG, "socket failed");
            ret = TINY_RET_E_SOCKET_FD;

#ifdef _WIN32
            DWORD e = GetLastError();
            LOG_D(TAG, "socket, e = %d", e);
#else
            LOG_D(TAG, "socket: %s", strerror(errno));
#endif
            break;
        }

        ret = tiny_join_multicast_group(thiz->fd, ip, group, port);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "tiny_join_multicast_group failed");
            break;
        }

        if (block)
        {
            break;
        }

        ret = tiny_socket_set_nonblock(thiz->fd);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "tiny_socket_set_nonblock failed");
            break;
        }
    } while (0);

    return ret;
}

TinyRet TinyMulticastSocket_Close(TinyMulticastSocket *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    tiny_leave_multicast_group(thiz->fd);

#ifdef _WIN32
    closesocket(thiz->fd);
#else
    shutdown(thiz->fd, SHUT_RDWR);
    close(thiz->fd);
#endif

    return TINY_RET_OK;
}
