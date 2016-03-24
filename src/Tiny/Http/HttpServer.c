/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   air_http_server.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "air_http_server.h"
#include "TinySocketIpc.h"
#include "TinyThread.h"
#include "air_selector.h"
#include "HttpConnPool.h"
#include "tiny_memory.h"

/*-----------------------------------------------------------------------------
 *
 * Private API declare
 *
 *-----------------------------------------------------------------------------*/
// for loop
static void * server_select_loop(void *param);
static TinyRet server_select_loop_once(HttpServer *thiz);
static void server_select_loop_stop(HttpServer *thiz);
static TinyRet pre_select(HttpServer *thiz);
static TinyRet post_select(HttpServer *thiz);

// for server socket
static TinyRet server_socket_init(HttpServer * thiz);
static TinyRet server_socket_prepare(HttpServer *thiz, uint16_t port);
static TinyRet server_socket_close(HttpServer *thiz);

#define MAX_LISTEN_CONNS        32
#define INIT_CONNECTION_ID      10000

typedef enum _Status
{
    STOPPED       = 0,
    RUNNING       = 1,
} Status;

struct _HttpServer
{
    uint32_t                    ref;
    Status                      status;

    int                         socket_fd;
    uint16_t                    listen_port;

    TinySocketIpc                    * ipc;

    HttpConnListener       on_conn_listener;
    void                      * on_conn_ctx;
    uint32_t                    conn_id;

    TinyThread                 * thread;
    AirSelector               * selector;

    HttpConnPool           * conn_pool;
};  

/*-----------------------------------------------------------------------------
 *
 * Public API
 *
 *-----------------------------------------------------------------------------*/
HttpServer * HttpServer_new(void)
{
    HttpServer *thiz = (HttpServer *)tiny_malloc(sizeof(HttpServer));
    if (thiz != NULL)
    {
        memset(thiz, 0, sizeof(HttpServer));
        thiz->ref = 1;
        thiz->status = STOPPED; 

        thiz->socket_fd = 0;
        thiz->listen_port = 0;

        thiz->ipc = TinySocketIpc_new();
        if (thiz->ipc == NULL)
        {
            air_free(thiz);
            return NULL;
        }

        thiz->on_conn_listener = NULL;
        thiz->on_conn_ctx = NULL;
        thiz->conn_id = INIT_CONNECTION_ID;

        thiz->thread = NULL;

        thiz->selector = air_selector_new();
        if (thiz->selector == NULL)
        {
            TinySocketIpc_release(thiz->ipc);
            air_free(thiz);
            return NULL;
        }

        thiz->conn_pool = HttpConnPool_New();
        if (thiz->conn_pool == NULL)
        {
            TinySocketIpc_release(thiz->ipc);
            air_selector_release(thiz->selector);
            air_free(thiz);
            return NULL;
        }
    }

    return thiz;
}

HttpServer * HttpServer_clone(HttpServer *thiz)
{
    return_val_if_fail(thiz, NULL);

    thiz->ref++;
    return thiz;
}

void HttpServer_release(HttpServer *thiz)
{
    return_if_fail(thiz);

    if (thiz->ref > 1)
    {
        thiz->ref --;
        return;
    }

    HttpServer_stop(thiz);

    TinySocketIpc_release(thiz->ipc);
    air_selector_release(thiz->selector);
    HttpConnPool_Delete(thiz->conn_pool);

    air_free(thiz);
}

TinyRet HttpServer_start(HttpServer *thiz, uint16_t port, HttpConnListener listener, void *ctx)
{
    TinyRet ret = RET_OK;

    return_val_if_fail(thiz, RET_ERR_NULL);

    if (thiz->status == STOPPED)
    {
        thiz->status = RUNNING;

        thiz->on_conn_listener = listener;
        thiz->on_conn_ctx = ctx;

        ret = server_socket_init(thiz);
        if (ret != RET_OK)
        {
            thiz->status = STOPPED;
            return ret;
        }

        ret = server_socket_prepare(thiz, port);
        if (ret != RET_OK)
        {
            server_socket_close(thiz);
            thiz->status = STOPPED;
            return ret;
        }

        // start select thread
        thiz->thread = TinyThread_new(server_select_loop, thiz, "tcp_server");
        if (thiz->thread == NULL)
        {
            thiz->status = STOPPED;
            server_socket_close(thiz);
            return RET_ERR_CALL_FAILED;
        }

        TinyThread_start(thiz->thread);
        return RET_OK;
    }

    return RET_ERR_ALREADY_START;
}

TinyRet HttpServer_stop(HttpServer *thiz)
{
    return_val_if_fail(thiz, RET_ERR_NULL);

    if (thiz->status == RUNNING)
    {
        thiz->status = STOPPED; 

        server_select_loop_stop(thiz);

        TinyThread_join(thiz->thread);
        TinyThread_release(thiz->thread);
        thiz->thread = NULL;

        HttpConnPool_Clean(thiz->conn_pool);

        return RET_OK;
    }

    return RET_ERR_NOT_START;
}

uint16_t HttpServer_GetListeningPort(HttpServer *thiz)
{
    return_val_if_fail(thiz, 0);

    if (thiz->listen_port == 0)
    {
        thiz->listen_port = air_socket_get_port(thiz->socket_fd);
    }

    return thiz->listen_port;
}

/*-----------------------------------------------------------------------------
 *
 * Private API - for loop
 *
 *-----------------------------------------------------------------------------*/
static void * server_select_loop(void *param)
{
    TinyRet ret = RET_OK;
    HttpServer *thiz = (HttpServer *)param;

    while (1)
    {
        ret = server_select_loop_once(thiz);
        if (ret != RET_OK)
            break;
    }

    return NULL;
}

static TinyRet server_select_loop_once(HttpServer *thiz)
{
    TinyRet ret = RET_OK;
    SelectorRet result = SELECTOR_RET_OK;

    ret = pre_select(thiz);
    if (ret != RET_OK)
    {
        return ret;
    }

    result = air_selector_run_once(thiz->selector, 0);
    if (result == SELECTOR_RET_ERROR)
    {
        return RET_ERR_CALL_FAILED;
    }

    ret = post_select(thiz);
    if (ret != RET_OK)
    {
        return ret;
    }

    return RET_OK;
}

static void server_select_loop_stop(HttpServer *thiz)
{
    TinySocketIpc_send_stop_msg(thiz->ipc);
}

/*-----------------------------------------------------------------------------
 *
 * Private API - for select
 *
 *-----------------------------------------------------------------------------*/

static TinyRet pre_select(HttpServer *thiz)
{
    int index = 0;
    int count = 0;

    air_selector_reset(thiz->selector);

    air_selector_set_read(thiz->selector, thiz->socket_fd);
    air_selector_set_read(thiz->selector, TinySocketIpc_get_socket_fd(thiz->ipc));

    return RET_OK;
}

static TinyRet post_select(HttpServer *thiz)
{
    int index = 0;
    int count = 0;

    if (air_selector_is_readable(thiz->selector, TinySocketIpc_get_socket_fd(thiz->ipc)) == YES)
    {
        IpcMsg m;
        memset(&m, 0, sizeof(IpcMsg));

        if (TinySocketIpc_recv(thiz->ipc, &m) == RET_OK)
        {
            if (m.type == IPC_MSG_STOP)
                return RET_ERR;

            if (m.type == IPC_MSG_RESELECT)
                return RET_OK;
        }

        return RET_OK;
    }

    if (air_selector_is_readable(thiz->selector, thiz->socket_fd) == YES)
    {
        int socket_fd = 0;
        struct sockaddr_in addr;
        socklen_t len = (socklen_t) sizeof(addr);

        memset(&addr, 0, sizeof(addr));
        socket_fd = accept(thiz->socket_fd, (struct sockaddr *)&addr, &len);
        if (socket_fd > 0)
        {
            const char *ip = inet_ntoa(addr.sin_addr);
            uint16_t port = ntohs(addr.sin_port);
            HttpConn *conn = HttpConn_New(socket_fd, ip, port, thiz->on_conn_listener, thiz->on_conn_ctx);
            if (conn != NULL)
            {
                HttpConn_SetId(conn, thiz->conn_id++);
                HttpConnPool_Add(thiz->conn_pool, conn);
                HttpConnPool_CleanClosed(thiz->conn_pool);
                return RET_OK;
            }
        }

        return RET_ERR_ACCEPT;
    }

    return RET_OK;
}

/*-----------------------------------------------------------------------------
 *
 * Private API - for Accept socket
 *
 *-----------------------------------------------------------------------------*/
static TinyRet server_socket_init(HttpServer * thiz)
{
    return_val_if_fail(thiz, RET_ERR_NULL);

    thiz->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (thiz->socket_fd < 0)
    {
        LOGD("socket failed");
        return RET_ERR_CALL_FAILED;
    }

    if (air_socket_set_nonblock(thiz->socket_fd) != RET_OK)
    {
        LOGD("air_socket_set_nonblock failed");
        return RET_ERR_CALL_FAILED;
    }

    return RET_OK;
}

static TinyRet server_socket_prepare(HttpServer *thiz, uint16_t port)
{
    int ret = 0;
    struct sockaddr_in  self_addr;

    return_val_if_fail(thiz, RET_ERR_NULL);

    memset(&self_addr, 0, sizeof(self_addr));
    self_addr.sin_family = AF_INET;
    self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    self_addr.sin_port = htons(port);

    ret = bind(thiz->socket_fd, (struct sockaddr *)&self_addr, sizeof(self_addr));

#ifdef _WIN32
    if (ret == SOCKET_ERROR)
    {
        return RET_ERR_CALL_FAILED;
    }
#else
    if (ret < 0)
    {
        return RET_ERR_CALL_FAILED;
    }
#endif

    ret = listen(thiz->socket_fd, MAX_LISTEN_CONNS);

#ifdef _WIN32
    if (ret == SOCKET_ERROR)
    {
        return RET_ERR_CALL_FAILED;
    }
#else
    if (ret < 0)
    {
        return RET_ERR_CALL_FAILED;
    }
#endif

    thiz->listen_port = (port == 0) ? air_socket_get_port(thiz->socket_fd) : port;

    return RET_OK;
}

static TinyRet server_socket_close(HttpServer *thiz)
{
    if (thiz->socket_fd == 0)
        return RET_ERR_socket_fd;
 
#ifdef _WIN32
    closesocket(thiz->socket_fd);
#else
    shutdown(thiz->socket_fd, SHUT_RD);
    close(thiz->socket_fd);
#endif

    thiz->socket_fd = 0;

    return RET_OK;
}
