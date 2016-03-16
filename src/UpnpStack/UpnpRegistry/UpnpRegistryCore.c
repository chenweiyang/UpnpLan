/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpRegistryCore.c
*
* @remark
*
*/

#include "UpnpRegistryCore.h"
#include "upnp_define.h"
#include "tiny_socket.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG                 "UpnpRegistryCore"

static TinyRet UpnpRegistryCore_SendMessage(UpnpRegistryCore *thiz, HttpMessage *message, int fd, const char *ip, uint16_t port);
static TinyRet open_sockets(UpnpRegistryCore *thiz);
static TinyRet close_sockets(UpnpRegistryCore *thiz);
static void core_loop(void *param);
static TinyRet core_pre_select(UpnpRegistryCore *thiz, uint32_t *timeout);
static bool core_select_once(UpnpRegistryCore *thiz, uint32_t timeout);
static void core_handle_message(UpnpRegistryCore *thiz, const char *buf, size_t len, const char *ip, uint16_t port);

UpnpRegistryCore * UpnpRegistryCore_New(void)
{
    UpnpRegistryCore *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (UpnpRegistryCore *)tiny_malloc(sizeof(UpnpRegistryCore));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpRegistryCore_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpRegistryCore_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet UpnpRegistryCore_Construct(UpnpRegistryCore *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpRegistryCore));
        thiz->running = false;
        thiz->group_fd = 0;
        thiz->search_fd = 0;
        thiz->requestHandler = NULL;
        thiz->responseHandler = NULL;
        thiz->ctx = NULL;

        ret = TinyThread_Construct(&thiz->thread);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = TinyThread_Initialize(&thiz->thread, core_loop, thiz, "UpnpRegistryCore");
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

TinyRet UpnpRegistryCore_Dispose(UpnpRegistryCore *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    UpnpRegistryCore_Stop(thiz);

    TinySocketIpc_Dispose(&thiz->ipc);
    TinySelector_Dispose(&thiz->selector);
    TinyThread_Dispose(&thiz->thread);

    return TINY_RET_OK;
}

void UpnpRegistryCore_Delete(UpnpRegistryCore *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpRegistryCore_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet UpnpRegistryCore_Start(UpnpRegistryCore *thiz,
    HttpRequestHandler requestHandler,
    HttpResponseHandler responseHandler,
    void *ctx)
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

        ret = open_sockets(thiz);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "open_sockets failed");
            break;
        }

        thiz->running = true;
        thiz->requestHandler = requestHandler;
        thiz->responseHandler = responseHandler;
        thiz->ctx = ctx;

        TinyThread_Start(&thiz->thread);
    } while (0);

    return ret;
}

TinyRet UpnpRegistryCore_Stop(UpnpRegistryCore *thiz)
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

        ret = close_sockets(thiz);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "close_sockets failed");
            break;
        }

        thiz->running = false;

        TinyThread_Join(&thiz->thread);
    } while (0);

    return ret;
}

static TinyRet UpnpRegistryCore_SendMessage(UpnpRegistryCore *thiz, HttpMessage *message, int fd, const char *ip, uint16_t port)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(ip, TINY_RET_E_ARG_NULL);

    do
    {
        char *bytes = NULL;
        uint32_t len = 0;

        if (!thiz->running)
        {
            LOG_D(TAG, "invalid operation, UpnpRegistryCore NOT Start");
            ret = TINY_RET_E_STOPPED;
            break;
        }

        ret = HttpMessage_ToBytes(message, &bytes, &len);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "HttpMessage_ToBytes: %s", tiny_ret_to_str(ret));
            break;
        }

        tiny_udp_write(fd, ip, port, bytes, len);
        tiny_free(bytes);
    } while (0);

    return ret;
}

TinyRet UpnpRegistryCore_Notify(UpnpRegistryCore *thiz, HttpMessage *message)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);

    return UpnpRegistryCore_SendMessage(thiz, message, thiz->group_fd, UPNP_GROUP, UPNP_PORT);
}

TinyRet UpnpRegistryCore_SendResponseTo(UpnpRegistryCore *thiz, HttpMessage *message)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);

    return UpnpRegistryCore_SendMessage(thiz, message, thiz->group_fd, HttpMessage_GetIp(message), HttpMessage_GetPort(message));
}

TinyRet UpnpRegistryCore_SendRequest(UpnpRegistryCore *thiz, HttpMessage *message)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);

    return UpnpRegistryCore_SendMessage(thiz, message, thiz->search_fd, UPNP_GROUP, UPNP_PORT);
}

TinyRet UpnpRegistryCore_SendRequestTo(UpnpRegistryCore *thiz, HttpMessage *message)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(message, TINY_RET_E_ARG_NULL);

    return UpnpRegistryCore_SendMessage(thiz, message, thiz->group_fd, HttpMessage_GetIp(message), HttpMessage_GetPort(message));
}

static TinyRet open_sockets(UpnpRegistryCore *thiz)
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

static TinyRet close_sockets(UpnpRegistryCore *thiz)
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

static void core_loop(void *param)
{
    TinyRet ret = TINY_RET_OK;
    UpnpRegistryCore *thiz = (UpnpRegistryCore *)param;

    while (thiz->running)
    {
        uint32_t timeout = 0;

        ret = core_pre_select(thiz, &timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (!core_select_once(thiz, timeout))
        {
            break;
        }
    }
}

static TinyRet core_pre_select(UpnpRegistryCore *thiz, uint32_t *timeout)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    TinySelector_Reset(&thiz->selector);
    TinySelector_Register(&thiz->selector, thiz->group_fd, SELECTOR_OP_READ);
    TinySelector_Register(&thiz->selector, thiz->search_fd, SELECTOR_OP_READ);
    TinySelector_Register(&thiz->selector, TinySocketIpc_GetFd(&thiz->ipc), SELECTOR_OP_READ);

    *timeout = 0;

    return TINY_RET_OK;
}

static bool core_select_once(UpnpRegistryCore *thiz, uint32_t timeout)
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

            if (thiz->requestHandler != NULL || thiz->responseHandler != NULL) 
            {
                core_handle_message(thiz, buf, bytes_read, ip, port);
            }
        }
    } while (0);

    return select_result;
}

static void core_handle_message(UpnpRegistryCore *thiz, const char *buf, size_t len, const char *ip, uint16_t port)
{
    HttpMessage *msg = NULL;

    do
    {
        HttpType type = HTTP_UNDEFINED;
        const char *method = NULL;
        const char *uri = NULL;
        TinyRet ret = TINY_RET_OK;

        msg = HttpMessage_New();
        if (msg == NULL)
        {
            LOG_D(TAG, "HttpMessage_New failed");
            break;
        }

        ret = HttpMessage_Parse(msg, buf, len);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "HttpMessage_Parse: invliad message");
            break;
        }

        HttpMessage_SetIp(msg, ip);
        HttpMessage_SetPort(msg, port);

        switch (HttpMessage_GetType(msg))
        {
        case HTTP_REQUEST:
            if (thiz->requestHandler != NULL)
            {
                thiz->requestHandler(msg, thiz->ctx);
            }
            break;

        case HTTP_RESPONSE:
            if (thiz->responseHandler != NULL)
            {
                thiz->responseHandler(msg, thiz->ctx);
            }
            break;

        default:
            LOG_D(TAG, "parse message: invliad HTTP type");
            break;
        }
    } while (0);

    if (msg != NULL)
    {
        HttpMessage_Delete(msg);
    }
}