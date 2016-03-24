/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpConn.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "HttpConn.h"
#include "TinySocketIpc.h"
#include "TinyThread.h"
#include "tiny_memory.h"

/*-----------------------------------------------------------------------------
 *
 * Private API declare
 *
 *-----------------------------------------------------------------------------*/
static void * conn_loop(void *param);

static TinyRet socket_init(HttpConn *thiz);
static TinyRet socket_close(HttpConn *thiz);
static TinyRet socket_waiting_for_read(HttpConn *thiz, uint32_t timeout);
static TinyRet socket_waiting_for_write(HttpConn *thiz, uint32_t timeout);
static int socket_read_once(HttpConn *thiz, char buf[], uint32_t buf_len);
static int socket_read_data(int socket_fd, char buf[], uint32_t len);
static int socket_write_once(HttpConn *thiz, const char *buf, uint32_t len);
static TinyRet socket_send_data(HttpConn *thiz, const char *bytes, uint32_t size, uint32_t timeout);

struct _HttpConn
{
    uint32_t            ref;
    uint32_t            id;
    HttpConnStatus      status;
    int                 socket_fd;
    TinySocketIpc     * ipc;
    uint32_t            recv_buf_size;

    char                self_ip[TINY_IP_LEN];
    char                client_ip[TINY_IP_LEN];
    uint16_t            client_port;

    TinyThread        * thread;
    HttpConnListener    listener;
    void              * ctx;
};

/*-----------------------------------------------------------------------------
 *
 * Public API
 *
 *-----------------------------------------------------------------------------*/
HttpConn * HttpConn_New(int socket_fd, 
                                const char *ip, 
                                uint16_t port, 
                                HttpConnListener listener,
                                void *ctx)
{
    HttpConn * thiz = (HttpConn *) tiny_malloc(sizeof(HttpConn));
    if (thiz != NULL)
    {
        thiz->ref = 1;
        thiz->id = 0;
        thiz->status = HTTP_CONN_CONNECTED;
        thiz->socket_fd = socket_fd;
        thiz->recv_buf_size = HTTP_RECV_BUF_LEN;

        strncpy(thiz->client_ip, ip, TINY_IP_LEN);
        thiz->client_port = port;

        thiz->ipc = TinySocketIpc_new();
        if (thiz->ipc == NULL)
        {
            air_free(thiz);
            return NULL;
        }

        thiz->on_listener = listener;
        thiz->ctx = ctx;

        // start thread
        thiz->thread = TinyThread_new(conn_loop, thiz, "http_conn");
        if (thiz->thread == NULL)
        {
            TinySocketIpc_release(thiz->ipc);
            air_free(thiz);
            return NULL;
        }

        TinyThread_start(thiz->thread);
    }

    return thiz;
}

void HttpConn_Delete(HttpConn *thiz)
{
    return_if_fail(thiz);

    HttpConn_Disconnect(thiz);
    TinySocketIpc_release(thiz->ipc);

    TinyThread_join(thiz->thread);

    air_free(thiz);
}

int16_t HttpConn_GetSocketFd(HttpConn * thiz)
{
    return_val_if_fail(thiz, 0);
    
    return thiz->socket_fd;
}

void HttpConn_SetBufferSize(HttpConn *thiz, uint32_t size)
{
    return_if_fail(thiz);

    thiz->recv_buf_size = size;
}

uint32_t HttpConn_GetBufferSize(HttpConn *thiz)
{
    return_val_if_fail(thiz, 0);

    return thiz->recv_buf_size;
}

TinyRet HttpConn_Disconnect(HttpConn *thiz)
{
    return_val_if_fail(thiz, HTTP_RET_ERR_NULL);

    if (thiz->status == HTTP_CONN_DISCONNECT)
        return HTTP_RET_ERR_DISCONNECT;

    TinySocketIpc_send_stop_msg(thiz->ipc);

    socket_close(thiz);

    return HTTP_RET_OK;
}

HttpConnStatus HttpConn_GetStatus(HttpConn *thiz)
{
    return_val_if_fail(thiz, HTTP_CONN_DISCONNECT);

    return thiz->status;
}

uint32_t HttpConn_GetIntAddress(HttpConn *thiz)
{
    return_val_if_fail(thiz, 0);

    return air_socket_get_ipv4_bytes(thiz->socket_fd);
}

const char * HttpConn_GetAddress(HttpConn *thiz)
{
    return_val_if_fail(thiz, NULL);

    air_socket_get_ip(thiz->socket_fd, thiz->self_ip, TINY_IP_LEN);
    return thiz->self_ip;
}

const char * HttpConn_GetClientAddress(HttpConn *thiz)
{
    return_val_if_fail(thiz, NULL);

    return thiz->client_ip;
}

uint16_t HttpConn_GetClientPort(HttpConn *thiz)
{
    return_val_if_fail(thiz, 0);

    return thiz->client_port;
}

void HttpConn_SetId(HttpConn *thiz, uint32_t id)
{
    return_if_fail(thiz);

    thiz->id = id;
}

uint32_t HttpConn_GetId(HttpConn * thiz)
{
    return_val_if_fail(thiz, 0);

    return thiz->id;
}

TinyRet HttpConn_SendMessage(HttpConn *thiz, AirHttpMsg *msg, uint32_t timeout)
{
    const char *bytes = NULL;
    uint32_t size = 0;

    return_val_if_fail(thiz, HTTP_RET_ERR_NULL);
    return_val_if_fail(msg, HTTP_RET_ERR_NULL);

    if (thiz->status != HTTP_CONN_CONNECTED)
        return HTTP_RET_ERR_DISCONNECT;

    bytes = HttpMessage_to_bytes(msg);
    size = HttpMessage_get_bytes_length(msg);

    return socket_send_data(thiz, bytes, size, timeout);
}

TinyRet HttpConn_SendContent(HttpConn *thiz, AirHttpContent *content, uint32_t timeout)
{
    const char *bytes = NULL;
    uint32_t size = 0;

    return_val_if_fail(thiz, HTTP_RET_ERR_NULL);
    return_val_if_fail(content, HTTP_RET_ERR_NULL);

    if (thiz->status != HTTP_CONN_CONNECTED)
        return HTTP_RET_ERR_DISCONNECT;

    bytes = air_http_content_get_data(content);
    size = air_http_content_get_size(content);

    return socket_send_data(thiz, bytes, size, timeout);
}

TinyRet HttpConn_RecvMessage(HttpConn *thiz, AirHttpMsg *msg, uint32_t timeout)
{
    TinyRet result = HTTP_RET_OK;
    TinyRet ret = RET_OK;
    char * buf = NULL;
    int size = 0;

    return_val_if_fail(thiz, HTTP_RET_ERR_NULL);
    return_val_if_fail(msg, HTTP_RET_ERR_NULL);

    if (thiz->status != HTTP_CONN_CONNECTED)
        return HTTP_RET_ERR_DISCONNECT;

    ret = socket_waiting_for_read(thiz, timeout);
    if (ret == RET_OK)
    {
        buf = (char *) tiny_malloc(thiz->recv_buf_size);
        if (buf == NULL)
            return HTTP_RET_ERR_MALLOC;

        memset(buf, 0, thiz->recv_buf_size);

        size = socket_read_once(thiz, buf, thiz->recv_buf_size);
        if (size > 0)
        {
            result = (HttpMessage_load(msg, buf, size) == HTTP_LOAD_OK) ?
                    HTTP_RET_OK : HTTP_RET_ERR_INVALID_DATA;

            HttpMessage_set_ip(msg, thiz->client_ip);
            HttpMessage_set_port(msg, thiz->client_port);
        }
        else 
        {
            result = HTTP_RET_ERR_READ;
        }

        air_free(buf);
        return result;
    }
    else if(ret == RET_ERR_TIMEOUT)
    {
        return HTTP_RET_ERR_TIMEOUT;
    }

    return HTTP_RET_ERR;
}

TinyRet HttpConn_RecvMessage(HttpConn *thiz, AirHttpContent *content, uint32_t timeout)
{
    TinyRet result = HTTP_RET_OK;
    TinyRet ret = RET_OK;
    char * buf = NULL;
    int size = 0;

    return_val_if_fail(thiz, HTTP_RET_ERR_NULL);
    return_val_if_fail(content, HTTP_RET_ERR_NULL);

    if (thiz->status != HTTP_CONN_CONNECTED)
        return HTTP_RET_ERR_DISCONNECT;

    ret = socket_waiting_for_read(thiz, timeout);
    if (ret == RET_OK)
    {
        buf = (char *) tiny_malloc(thiz->recv_buf_size);
        if (buf != NULL)
        {
            memset(buf, 0, thiz->recv_buf_size);

            size = socket_read_once(thiz, buf, thiz->recv_buf_size);
            if (size > 0)
            {
                result = (air_http_content_set_data(content, buf, size) == RET_OK) ?
                    HTTP_RET_OK : HTTP_RET_ERR_INVALID_DATA;
            }
            else
            {
                result = HTTP_RET_ERR_READ;
            }

            air_free(buf);
            return result;
        }

        return HTTP_RET_ERR_MALLOC;
    }
    else if(ret == RET_ERR_TIMEOUT)
    {
        return HTTP_RET_ERR_TIMEOUT;
    }

    return HTTP_RET_ERR;
}

TinyRet HttpConn_Recv(HttpConn *thiz, HttpConn_RecvHandler on_recv, void *ctx, uint32_t timeout)
{
    TinyRet result = HTTP_RET_OK;
    TinyRet ret = RET_OK;
    char * buf = NULL;
    int size = 0;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      

    return_val_if_fail(thiz, HTTP_RET_ERR_NULL);
    return_val_if_fail(on_recv, HTTP_RET_ERR_NULL);

    if (thiz->status != HTTP_CONN_CONNECTED)
        return HTTP_RET_ERR_DISCONNECT;

    ret = socket_waiting_for_read(thiz, timeout);
    if (ret == RET_OK)
    {
        buf = (char *) tiny_malloc(thiz->recv_buf_size);
        if (buf != NULL)
        {
            while (1)
            {
                memset(buf, 0, thiz->recv_buf_size);
                size = socket_read_once(thiz, buf, thiz->recv_buf_size);
                if (size > 0)
                {
                    on_recv(thiz, buf, size, ctx);
                }
                else
                {
                    break;
                }
            }

            air_free(buf);
            return HTTP_RET_OK;
        }

        return HTTP_RET_ERR_MALLOC;
    }
    else if(ret == RET_ERR_TIMEOUT)
    {
        return HTTP_RET_ERR_TIMEOUT;
    }

    return HTTP_RET_ERR;
}

/*-----------------------------------------------------------------------------
 *
 * Private API - for connection socket
 *
 *-----------------------------------------------------------------------------*/

static TinyRet socket_init(HttpConn *thiz)
{
    thiz->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (thiz->socket_fd < 0)
    {
        LOGD("socket failed");
        return RET_ERR_CALL_FAILED;
    }

    if (air_socket_set_nonblock(thiz->socket_fd) != RET_OK)
    {
        LOGD("air_set_nonblock failed");
        return RET_ERR_CALL_FAILED;
    }

    return RET_OK;
}

static TinyRet socket_close(HttpConn *thiz)
{
    if (thiz->status != HTTP_CONN_DISCONNECT)
    {
    #ifdef _WIN32
        closesocket(thiz->socket_fd);
    #else
        shutdown(thiz->socket_fd, SHUT_RDWR);
        close(thiz->socket_fd);
   #endif

        thiz->status = HTTP_CONN_DISCONNECT;
    }

    return RET_OK;
}

static TinyRet socket_waiting_for_read(HttpConn *thiz, uint32_t timeout)
{
    int ret = 0;
    int max_fd = 0;
    fd_set read_set;
    struct timeval tv;

    FD_ZERO(&read_set);  
    FD_SET(thiz->socket_fd, &read_set);
    FD_SET(TinySocketIpc_get_socket_fd(thiz->ipc), &read_set);

    /*
     * NOTE
     *   max_fd is not used on Windows,
     *   but on linux/unix it MUST Greater than socket_fd.
     */
    if (max_fd <= thiz->socket_fd)
        max_fd = thiz->socket_fd + 1;

    if (timeout > 0) {
        tv.tv_sec = timeout / 1000;
        tv.tv_usec = timeout % 1000;

        ret = select(max_fd, &read_set, NULL, NULL, &tv);
    }
    else {
        ret = select(max_fd, &read_set, NULL, NULL, NULL);
    }

    if (ret < 0)
    {
        LOGI("error: select");
        return RET_ERR_SELECT;
    }

    if (ret == 0)
        return RET_ERR_TIMEOUT;

    if (FD_ISSET(thiz->socket_fd, &read_set))
        return RET_OK;

    if (FD_ISSET(TinySocketIpc_get_socket_fd(thiz->ipc),  &read_set))
    {
        IpcMsg m;
        memset(&m, 0, sizeof(IpcMsg));

        if (TinySocketIpc_recv(thiz->ipc, &m) == RET_OK)
        {
            if (m.type == IPC_MSG_STOP)
                return RET_ERR_CLOSED_FD;
        }
    }

    return RET_ERR;
}

static int socket_read_once(HttpConn *thiz, char buf[], uint32_t buf_len)
{
    socklen_t len = 0;
#ifdef _WIN32
    DWORD error = 0;
#else
    char error = 0;
#endif

    LOGD("socket_read_once");

    len = sizeof (error);
    if (getsockopt(thiz->socket_fd, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0)
        return -1;

    if (error)
        return -1;

    return socket_read_data(thiz->socket_fd, buf, buf_len);
}

static int socket_read_data(int socket_fd, char buf[], uint32_t len)
{
    uint32_t received = 0;
    
    LOGD("socket_read_data");

    while (len > 0)
    {
        int n = recv(socket_fd, buf + received, len , 0);
        LOGD("recv: %d", n);

        if (n == 0)
            break;

    #ifdef _WIN32
        if (n == SOCKET_ERROR)
        {
            break;
        #if 0
            DWORD e = GetLastError();
            LOGI("GetLastError: %d", e);

            if (e == WSAEWOULDBLOCK)
            {
                break;
                air_usleep(100);
                continue;
            }
            else
            {
                break;
            }
        #endif
        }
    #else
        if (n == -1)
        {
            // more data
            if (errno == EAGAIN)
                continue;
            else
                break;
        }
    #endif

        received += n;
        len -= n;

        // 2013.7.11 如果再继续读，Windows下出现10035错误，然后是recv死循环，原因待查
        // break;
    }

    return (received == 0) ? -1 : received;
}

static TinyRet socket_waiting_for_write(HttpConn *thiz, uint32_t timeout)
{
    int ret = 0;
    int max_fd = 0;
    fd_set read_set;
    fd_set write_set;
    struct timeval tv;

    FD_ZERO(&read_set);  
    FD_SET(TinySocketIpc_get_socket_fd(thiz->ipc), &read_set);

    FD_ZERO(&write_set);  
    FD_SET(thiz->socket_fd, &write_set);

    /*
     * NOTE
     *   max_fd is not used on Windows,
     *   but on linux/unix it MUST Greater than socket_fd.
     */
    if (max_fd <= thiz->socket_fd)
        max_fd = thiz->socket_fd + 1;

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = timeout % 1000;

    ret = select(max_fd, &read_set, &write_set, NULL, &tv);
    if (ret < 0)
    {
        LOGI("error: select");
        return RET_ERR_SELECT;
    }

    if (ret == 0)
        return RET_ERR_TIMEOUT;

    if (FD_ISSET(thiz->socket_fd, &write_set))
        return RET_OK;

    if (FD_ISSET(TinySocketIpc_get_socket_fd(thiz->ipc),  &read_set))
    {
        IpcMsg m;
        memset(&m, 0, sizeof(IpcMsg));

        if (TinySocketIpc_recv(thiz->ipc, &m) == RET_OK)
        {
            if (m.type == IPC_MSG_STOP)
                return RET_ERR_CLOSED_FD;
        }
    }

    return RET_ERR;
}

static int socket_write_once(HttpConn *thiz, const char *buf, uint32_t len)
{
    int sent = 0;

    while (len > 0)
    {
        int n = send(thiz->socket_fd, buf + sent, len , 0);
        if (n == 0)
            break;

        LOGD("send bytes: %d", n);

    #ifdef _WIN32
        if (n == SOCKET_ERROR)
        {
            DWORD e = GetLastError();
            LOGI("GetLastError: %d", e);

            if (e == WSAEWOULDBLOCK)
            {
                air_usleep(100);
                continue;
            }
            else
            {
                break;
            }
        }
    #else
        if (n == -1)
        {
            // more data
            if (errno == EAGAIN)
                continue;
            else
                break;
        }
    #endif

        sent += n;
        len -= n;
    }

    return sent;
}

static TinyRet socket_send_data(HttpConn *thiz, const char *bytes, uint32_t size, uint32_t timeout)
{
    TinyRet ret = socket_waiting_for_write(thiz, timeout);
    if (ret == RET_OK)
    {
        if (socket_write_once(thiz, bytes, size) == size)
            return HTTP_RET_OK;

        return HTTP_RET_ERR_DISCONNECT;
    }
    else if(ret == RET_ERR_TIMEOUT)
    {
        return HTTP_RET_ERR_TIMEOUT;
    }

    return HTTP_RET_ERR;
}

/*-----------------------------------------------------------------------------
 *
 * Private API - for thread
 *
 *-----------------------------------------------------------------------------*/
static void * conn_loop(void *param)
{
    HttpConn *thiz = (HttpConn *)param;

    thiz->on_listener(thiz, thiz->ctx);

    socket_close(thiz);

    return NULL;
}