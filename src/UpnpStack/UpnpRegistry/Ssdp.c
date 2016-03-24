/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   Ssdp.c
*
* @remark
*
*/

#include "Ssdp.h"
#include "upnp_define.h"
#include "tiny_socket.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG                 "Ssdp"


static TinyRet Ssdp_Send(Ssdp *thiz, const char *bytes, uint32_t len, int fd, const char *ip, uint16_t port);
static TinyRet Ssdp_OpenSockets(Ssdp *thiz);
static TinyRet Ssdp_CloseSockets(Ssdp *thiz);
static void Ssdp_Loop(void *param);
static TinyRet Ssdp_PreSelect(Ssdp *thiz, uint32_t *timeout);
static bool Ssdp_SelectOnce(Ssdp *thiz, uint32_t timeout);
static void Ssdp_ProcessMessage(Ssdp *thiz, const char *buf, size_t len, const char *ip, uint16_t port);

Ssdp * Ssdp_New(void)
{
    Ssdp *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (Ssdp *)tiny_malloc(sizeof(Ssdp));
        if (thiz == NULL)
        {
            break;
        }

        ret = Ssdp_Construct(thiz);
        if (RET_FAILED(ret))
        {
            Ssdp_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet Ssdp_Construct(Ssdp *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(Ssdp));
        thiz->running = false;
        thiz->group_fd = 0;
        thiz->search_fd = 0;
        thiz->handler = NULL;
        thiz->ctx = NULL;

        ret = TinyThread_Construct(&thiz->thread);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyThread_Initialize(&thiz->thread, Ssdp_Loop, thiz, "Ssdp");
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinySelector_Construct(&thiz->selector);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinySocketIpc_Construct(&thiz->ipc);
        if (RET_FAILED(ret))
        {
            break;
        }
    } while (0);

    return ret;
}

void Ssdp_Dispose(Ssdp *thiz)
{
    RETURN_IF_FAIL(thiz);

    Ssdp_Stop(thiz);

    TinySocketIpc_Dispose(&thiz->ipc);
    TinySelector_Dispose(&thiz->selector);
    TinyThread_Dispose(&thiz->thread);
}

void Ssdp_Delete(Ssdp *thiz)
{
    RETURN_IF_FAIL(thiz);

    Ssdp_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet Ssdp_SetMessageHandler(Ssdp *thiz, SsdpMessageHandler handler, void *ctx)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(handler, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->running)
        {
            ret = TINY_RET_E_STARTED;
            break;
        }

        thiz->handler = handler;
        thiz->ctx = ctx;
    } while (0);

    return ret;
}

TinyRet Ssdp_Start(Ssdp *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (thiz->running)
        {
            ret = TINY_RET_E_STARTED;
            break;
        }

        ret = Ssdp_OpenSockets(thiz);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "Ssdp_OpenSockets failed");
            break;
        }

        thiz->running = true;

        TinyThread_Start(&thiz->thread);
    } while (0);

    return ret;
}

TinyRet Ssdp_Stop(Ssdp *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        if (!thiz->running)
        {
            ret = TINY_RET_E_STOPPED;
            break;
        }

        TinySocketIpc_SendStopMsg(&thiz->ipc);

        ret = Ssdp_CloseSockets(thiz);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "Ssdp_CloseSockets failed");
            break;
        }

        thiz->running = false;

        TinyThread_Join(&thiz->thread);
    } while (0);

    return ret;
}

static TinyRet Ssdp_Send(Ssdp *thiz, const char *bytes, uint32_t len, int fd, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(bytes, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ip, TINY_RET_E_ARG_NULL);

    do
    {
        if (!thiz->running)
        {
            LOG_D(TAG, "invalid operation, Ssdp NOT Start");
            ret = TINY_RET_E_STOPPED;
            break;
        }

        tiny_udp_write(fd, ip, port, bytes, len);
    } while (0);

    return ret;
}

TinyRet Ssdp_SendMessage(Ssdp *thiz, SsdpMessage *message)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        char string[SSDP_MSG_MAX_LEN];
        uint32_t len = 0;

        memset(string, 0, SSDP_MSG_MAX_LEN);

        len = SsdpMessage_ToString(message, string, SSDP_MSG_MAX_LEN);
        if (len == 0)
        {
            ret = TINY_RET_E_INTERNAL;
            break;
        }

        switch (message->type)
        {
        case SSDP_ALIVE:
        case SSDP_BYEBYE:
            break;

        case SSDP_MSEARCH_REQUEST:
            ret = Ssdp_Send(thiz, string, len, thiz->search_fd, UPNP_GROUP, UPNP_PORT);
            break;

        case SSDP_MSEARCH_RESPONSE:
            ret = Ssdp_Send(thiz, string, len, thiz->group_fd, message->ip, message->port);
            break;
        }
    } while (0);

    return ret;
}

static TinyRet Ssdp_OpenSockets(Ssdp *thiz)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ret = tiny_udp_multicast_open(&thiz->group_fd, UPNP_GROUP, UPNP_PORT, false);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "tiny_udp_multicast_open failed : %s", tiny_ret_to_str(ret));
            break;
        }

        ret = tiny_udp_unicast_open(&thiz->search_fd, 0, false);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "tiny_udp_unicast_open failed : %s", tiny_ret_to_str(ret));
            break;
        }
    } while (0);

    return ret;
}

static TinyRet Ssdp_CloseSockets(Ssdp *thiz)
{
    TinyRet ret = TINY_RET_OK;

    do
    {
        ret = tiny_udp_unicast_close(thiz->search_fd);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "tiny_udp_multicast_open failed : %s", tiny_ret_to_str(ret));
            break;
        }

        ret = tiny_udp_multicast_close(thiz->group_fd);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "tiny_udp_multicast_open failed : %s", tiny_ret_to_str(ret));
            break;
        }
    } while (0);

    return ret;
}

static void Ssdp_Loop(void *param)
{
    TinyRet ret = TINY_RET_OK;
    Ssdp *thiz = (Ssdp *)param;

    while (thiz->running)
    {
        uint32_t timeout = 0;

        ret = Ssdp_PreSelect(thiz, &timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (!Ssdp_SelectOnce(thiz, timeout))
        {
            break;
        }
    }
}

static TinyRet Ssdp_PreSelect(Ssdp *thiz, uint32_t *timeout)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinySelector_Reset(&thiz->selector);
    TinySelector_Register(&thiz->selector, thiz->group_fd, SELECTOR_OP_READ);
    TinySelector_Register(&thiz->selector, thiz->search_fd, SELECTOR_OP_READ);
    TinySelector_Register(&thiz->selector, TinySocketIpc_GetFd(&thiz->ipc), SELECTOR_OP_READ);

    *timeout = 0;

    return TINY_RET_OK;
}

static bool Ssdp_SelectOnce(Ssdp *thiz, uint32_t timeout)
{
    bool select_result = true;
    TinyRet ret = TINY_RET_OK;

    do
    {
        int fd = 0;
        TinySelectorRet result = SELECTOR_RET_OK;

        result = TinySelector_RunOnce(&thiz->selector, timeout);
        if (result == SELECTOR_RET_ERROR)
        {
            select_result = false;
            break;
        }

        if (result == SELECTOR_RET_TIMEOUT)
        {
            break;
        }

        if (TinySelector_IsReadable(&thiz->selector, TinySocketIpc_GetFd(&thiz->ipc)))
        {
            IpcMsg msg;
            memset(&msg, 0, sizeof(IpcMsg));

            ret = TinySocketIpc_Recv(&thiz->ipc, &msg);
            if (RET_FAILED(ret))
            {
                break;
            }

            if (msg.type == IPC_MSG_STOP)
            {
                select_result = false;
                break;
            }
        }

        if (TinySelector_IsReadable(&thiz->selector, thiz->group_fd))
        {
            fd = thiz->group_fd;
        }
        else if (TinySelector_IsReadable(&thiz->selector, thiz->search_fd))
        {
            fd = thiz->search_fd;
        }
        else
        {
            break;
        }

        if (fd > 0)
        {
            char ip[TINY_IP_LEN];
            uint16_t port;
            char buf[2048];
            int bytes_read = 0;

            bytes_read = tiny_udp_read(fd, buf, 2048, ip, TINY_IP_LEN, &port);
            if (bytes_read <= 0)
            {
                LOG_D(TAG, "tiny_udp_read failed");
                break;
            }

#if 0
            printf("%s\n", buf);
#endif

            Ssdp_ProcessMessage(thiz, buf, bytes_read, ip, port);
        }
    } while (0);

    return select_result;
}

static void Ssdp_ProcessMessage(Ssdp *thiz, const char *buf, size_t len, const char *ip, uint16_t port)
{
    SsdpMessage message;

    if (RET_FAILED(SsdpMessage_Construct(&message, ip, port, buf, len)))
    {
        return;
    }

    thiz->handler(&message, thiz->ctx);

    SsdpMessage_Dispose(&message);
}