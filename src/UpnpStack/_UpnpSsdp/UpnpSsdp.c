/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-7-9
 *
 * @file   UpnpSsdp.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include "UpnpSsdp.h"
#include "upnp_define.h"
#include "ct_memory.h"
#include "ct_socket.h"
#include "ct_log.h"
#include "HttpMessage.h"
#include "UpnpSsdpJob.h"
#include "UpnpObjectList.h"

#define TAG                 "UpnpSsdp"

/**
 * Presence
 * <p> NOTIFY * HTTP/1.1 </p>
 * <p> HOST: 239.255.255.250:1900 </p>
 * <p> CACHE-CONTROL: max-age=[Message lifetime] </p>
 * <p> LOCATION: [URL for the Device Description Document] </p>
 * <p> AL: <[application-specific URI]><[application-specific URI]> -> option </p>
 * <p> NT: [URI indicating the type of presence advertisement] </p>
 * <p> NTS: ssdp:alive </p>
 * <p> SERVER: [OS and version] UPnP/1.0 </p>
 * <p> USN: [URI identifying this message] </p>
 */

/**
 * Exit Presence
 * <p> NOTIFY * HTTP/1.1 </p>
 * <p> HOST: 239.255.255.250:1900 </p>
 * <p> NT: [URI indicating the type of exit message] </p>
 * <p> NTS: ssdp:byebye </p>
 * <p> USN: [URI identifying this message] </p>
 */

/*
* M-SEARCH request
* <p> M-SEARCH * HTTP/1.1 </p>
* <p> HOST: 239.255.255.250:1900 </p>
* <p> ST: [URI for search target] </p>
* <p> MAN: "ssdp:discover" </p>
* <p> MX: [Maximum response delay in seconds] </p>
*/

/* method */
#define METHOD_NOTIFY       "NOTIFY"
#define METHOD_MSEARCH      "M-SEARCH"

/* headers */
#define HEAD_HOST           "HOST"
#define HEAD_CACHE_CONTROL  "CACHE-CONTROL"
#define HEAD_LOCATION       "LOCATION"
#define HEAD_ST             "ST"
#define HEAD_MAN            "MAN"
#define HEAD_MX             "MX"
#define HEAD_AL             "AL"
#define HEAD_NT             "NT"
#define HEAD_NTS            "NTS"
#define HEAD_SERVER         "SERVER"
#define HEAD_USN            "USN"

/* host value */
#define DEFAULT_HOST        "239.255.255.250:1900"

/* MAN value */
#define DEFAULT_MAN         "\"ssdp:discover\""

/* ST value */
#define DEFAULT_ST          "ssdp:all"

/* MX value */
#define DEFAULT_MX          3

/* NTS value */
#define NTS_ALIVE           "ssdp:alive"
#define NTS_BYEBYE          "ssdp:byebye"

static UpnpSsdp * UpnpSsdp_New(void);
static CtRet UpnpSsdp_Construct(UpnpSsdp *thiz);
static CtRet UpnpSsdp_Dispose(UpnpSsdp *thiz);
static void UpnpSsdp_Delete(UpnpSsdp *thiz);

static void object_delete_listener(void * data, void *ctx);
static void ssdp_loop(void *param);
static CtRet open_sockets(UpnpSsdp *thiz);
static CtRet close_sockets(UpnpSsdp *thiz);
static CtRet ssdp_pre_select(UpnpSsdp *thiz, uint32_t *timeout);
static bool ssdp_select_once(UpnpSsdp *thiz, uint32_t timeout);

static bool worker_listener(CtWorker *worker, void *job, void *ctx);
static CtRet parse_message(UpnpSsdp *thiz, const char *buf, uint32_t len, const char *ip, uint16_t port);
static void parse_notify(UpnpSsdp *thiz, HttpMessage *msg, const char *ip);
static void parse_search_request(UpnpSsdp *thiz, HttpMessage *msg, const char *ip, uint16_t port);
static void parse_search_response(UpnpSsdp *thiz, HttpMessage *msg, const char *ip, uint16_t port);
static UpnpObject * convert_upnp_object(const char *ip, HttpMessage *msg, bool strict_uuid);
static bool validate_usn(UpnpSsdp *thiz, const char *usn);
static CtRet validate_nt(const char *type, bool strict_uuid);

struct _UpnpSsdp
{
    bool                    is_running;
    CtThread                thread;
    CtWorker                worker;
    bool                    strict_uuid;

    CtSelector              selector;
    CtSocketIpc             ipc;
    int                     fd;
    int                     search_fd;

    UpnpObjectFilter        objectFilter;
    UpnpObjectListener      objectListener;
    void                  * ctx;

    UpnpObjectList        * foundObjects;
    UpnpObjectList        * adverObjects;
};

static UpnpSsdp * theInstance = NULL;

UpnpSsdp * UpnpSsdp_GetInstance(void)
{
    if (theInstance == NULL)
    {
        theInstance = UpnpSsdp_New();
    }

    return theInstance;
}

static UpnpSsdp * UpnpSsdp_New(void)
{
    UpnpSsdp *thiz = NULL;

    do
    {
        CtRet ret = CT_RET_OK;

        thiz = (UpnpSsdp *)ct_malloc(sizeof(UpnpSsdp));
        if (thiz == NULL)
        {
            break;
        }

        ret = UpnpSsdp_Construct(thiz);
        if (RET_FAILED(ret))
        {
            UpnpSsdp_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

CtRet UpnpSsdp_Construct(UpnpSsdp *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(UpnpSsdp));
        thiz->strict_uuid = false;
        thiz->is_running = false;
        thiz->fd = 0;
        thiz->search_fd = 0;

        ret = CtThread_Construct(&thiz->thread);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = CtThread_Initialize(&thiz->thread, ssdp_loop, thiz, "UpnpSsdp");
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = ScWorker_Construct(&thiz->worker);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = CtSelector_Construct(&thiz->selector);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = CtSocketIpc_Construct(&thiz->ipc);
        if (RET_FAILED(ret))
        {
            break;
        }

        thiz->foundObjects = UpnpObjectList_New();
        if (thiz->foundObjects == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        thiz->adverObjects = UpnpObjectList_New();
        if (thiz->adverObjects == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }
    }
    while (0);

    return ret;
}

CtRet UpnpSsdp_Dispose(UpnpSsdp *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    UpnpSsdp_Stop(thiz);
    CtSocketIpc_Dispose(&thiz->ipc);
    CtSelector_Dispose(&thiz->selector);
    CtThread_Dispose(&thiz->thread);
    ScWorker_Dispose(&thiz->worker);

    UpnpObjectList_Delete(thiz->foundObjects);
    UpnpObjectList_Delete(thiz->adverObjects);

    return CT_RET_OK;
}

void UpnpSsdp_Delete(UpnpSsdp *thiz)
{
    RETURN_IF_FAIL(thiz);

    UpnpSsdp_Dispose(thiz);
    ct_free(thiz);
}

CtRet UpnpSsdp_SetUuidMode(UpnpSsdp *thiz, bool strict_uuid)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        if (thiz->is_running)
        {
            ret = CT_RET_E_STARTED;
            break;
        }
    
        thiz->strict_uuid = strict_uuid;
    } while (0);

    return ret;
}

CtRet UpnpSsdp_SetListener(UpnpSsdp *thiz, UpnpObjectListener listener, UpnpObjectFilter filter, void *ctx)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        if (thiz->is_running)
        {
            ret = CT_RET_E_STARTED;
            break;
        }

        thiz->objectFilter = filter;
        thiz->objectListener = listener;
        thiz->ctx = ctx;
    } while (0);

    return ret;
}

CtRet UpnpSsdp_Start(UpnpSsdp *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        if (thiz->is_running)
        {
            ret = CT_RET_E_STARTED;
            break;
        }

        ret = ScWorker_Start(&thiz->worker, "UpnpSsdp.worker", worker_listener, thiz);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "ScWorker_Start failed");
            break;
        }

        ret = open_sockets(thiz);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "open_sockets failed");
            break;
        }
 
        thiz->is_running = true;
        CtThread_Start(&thiz->thread);

        if (thiz->objectListener != NULL)
        {
            ret = UpnpSsdp_Search(thiz);
        }
    } while (0);

    return ret;
}

CtRet UpnpSsdp_Stop(UpnpSsdp *thiz)
{
    CtRet ret = CT_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        if (! thiz->is_running)
        {
            ret = CT_RET_E_STOPPED;
            break;
        }

        CtSocketIpc_SendStopMsg(&thiz->ipc);

        ret = close_sockets(thiz);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "close_sockets failed");
            break;
        }

        ScWorker_Stop(&thiz->worker);
        thiz->is_running = false;

        UpnpObjectList_Clear(thiz->foundObjects);
    } while (0);

    return ret;
}

CtRet UpnpSsdp_Search(UpnpSsdp *thiz)
{
    CtRet ret = CT_RET_OK;
    HttpMessage *msg = NULL;

    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);

    do
    {
        char *bytes = NULL;
        uint32_t len = 0;

        if (!thiz->is_running)
        {
            ret = CT_RET_E_STOPPED;
            break;
        }

        msg = HttpMessage_New();
        if (msg == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        HttpMessage_SetType(msg, HTTP_REQUEST);
        HttpMessage_SetVersion(msg, 1, 1);
        HttpMessage_SetMethod(msg, METHOD_MSEARCH);
        HttpMessage_SetUri(msg, "*");
        HttpMessage_SetHeader(msg, HEAD_HOST, DEFAULT_HOST);
        HttpMessage_SetHeader(msg, HEAD_ST, DEFAULT_ST);
        HttpMessage_SetHeader(msg, HEAD_MAN, DEFAULT_MAN);
        HttpMessage_SetHeaderInteger(msg, HEAD_MX, DEFAULT_MX);

        ret = HttpMessage_ToBytes(msg, &bytes, &len);
        if (RET_FAILED(ret))
        {
            break;
        }

#if 0
        printf("--- send search-> %s:%d ---\n", UPNP_GROUP, UPNP_PORT);
        printf("%s\n", bytes);
#endif

        ct_udp_write(thiz->search_fd, UPNP_GROUP, UPNP_PORT, bytes, len);
        ct_free(bytes);
    } while (0);

    if (msg != NULL)
    {
        HttpMessage_Delete(msg);
    }

    return ret;
}

CtRet UpnpSsdp_StartAdvertising(UpnpSsdp *thiz, UpnpObject *object)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(object, CT_RET_E_ARG_NULL);


    return CT_RET_E_NOT_IMPLEMENTED;
}

CtRet UpnpSsdp_StopAdvertising(UpnpSsdp *thiz, UpnpObject *object)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(object, CT_RET_E_ARG_NULL);


    return CT_RET_E_NOT_IMPLEMENTED;
}

static void objects_delete_listener(void * data, void *ctx)
{
    UpnpObject *objects = (UpnpObject *)data;
    UpnpObject_Delete(objects);
}

static CtRet open_sockets(UpnpSsdp *thiz)
{
    CtRet ret = CT_RET_OK;

    do
    {
        ret = ct_udp_multicast_open(&thiz->fd, UPNP_GROUP, UPNP_PORT, false);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "ct_udp_multicast_open failed : %s", ct_ret_to_str(ret));
            break;
        }

        ret = ct_udp_unicast_open(&thiz->search_fd, 0, false);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "ct_udp_unicast_open failed : %s", ct_ret_to_str(ret));
            break;
        }
    } while (0);

    return ret;
}

static CtRet close_sockets(UpnpSsdp *thiz)
{
    CtRet ret = CT_RET_OK;

    do
    {
        ret = ct_udp_unicast_close(thiz->search_fd);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "ct_udp_multicast_open failed : %s", ct_ret_to_str(ret));
            break;
        }

        ret = ct_udp_multicast_close(thiz->fd);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "ct_udp_multicast_open failed : %s", ct_ret_to_str(ret));
            break;
        }
    } while(0);

    return ret;
}

static void ssdp_loop(void *param)
{
    CtRet ret = CT_RET_OK;
    UpnpSsdp *thiz = (UpnpSsdp *)param;

    while (1)
    {
        uint32_t timeout = 0;

        ret = ssdp_pre_select(thiz, &timeout);
        if (RET_FAILED(ret))
        {
            break;
        }

        if (! ssdp_select_once(thiz, timeout))
        {
            break;
        }
    }
}

static CtRet ssdp_pre_select(UpnpSsdp *thiz, uint32_t *timeout)
{
    RETURN_VAL_IF_FAIL(thiz, CT_RET_E_ARG_NULL);
    
    CtSelector_Reset(&thiz->selector);
    CtSelector_Register(&thiz->selector, thiz->fd, SELECTOR_OP_READ);
    CtSelector_Register(&thiz->selector, thiz->search_fd, SELECTOR_OP_READ);
    CtSelector_Register(&thiz->selector, CtSocketIpc_GetFd(&thiz->ipc), SELECTOR_OP_READ);

    *timeout = 0;

    return CT_RET_OK;
}

static bool ssdp_select_once(UpnpSsdp *thiz, uint32_t timeout)
{
    bool select_result = true;
    CtRet ret = CT_RET_OK;

    do
    {
        int fd = 0;
        CtSelectorRet result = SELECTOR_RET_OK;
        
        result = CtSelector_RunOnce(&thiz->selector, timeout);
        if (result == SELECTOR_RET_ERROR)
        {
            select_result = false;
            break;
        }

        if (result == SELECTOR_RET_TIMEOUT)
        {
            break;
        }

        if (CtSelector_IsReadable(&thiz->selector, CtSocketIpc_GetFd(&thiz->ipc)))
        {
            IpcMsg msg;
            memset(&msg, 0, sizeof(IpcMsg));
            
            ret = CtSocketIpc_Recv(&thiz->ipc, &msg);
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

        if (CtSelector_IsReadable(&thiz->selector, thiz->fd))
        {
            fd = thiz->fd;
        }
        else if (CtSelector_IsReadable(&thiz->selector, thiz->search_fd))
        {
            fd = thiz->search_fd;
        }
        else
        {
            break;
        }

        if (fd > 0)
        {
            UpnpSsdpJob * job = UpnpSsdpJob_New();
            if (job == NULL)
            {
                LOG_E(TAG, "UpnpSsdpJob_New failed");
                break;
            }

            job->bytes_read = ct_udp_read(fd, job->buf, SSDP_JOB_BUF_LEN, job->ip, CT_IP_LEN, &job->port);
            if (job->bytes_read <= 0)
            {
                LOG_D(TAG, "ct_udp_read failed");
                UpnpSsdpJob_Delete(job);
                break;
            }

#if 0
            printf("%s\n", job->buf);
#endif

            ret = ScWorker_PutJob(&thiz->worker, job);
            if (RET_FAILED(ret))
            {
                LOG_E(TAG, "ScWorker_PutJob: %s", ct_ret_to_str(ret));
                UpnpSsdpJob_Delete(job);
                break;
            }
        }
    } while (0);

    return select_result;
}

static bool worker_listener(CtWorker *worker, void *job, void *ctx)
{
    UpnpSsdp *thiz = (UpnpSsdp *)ctx;
    UpnpSsdpJob * j = (UpnpSsdpJob *)job;

    parse_message(thiz, UpnpSsdpJob_GetBuffer(j), UpnpSsdpJob_GetBytesRead(j), UpnpSsdpJob_GeIp(j), UpnpSsdpJob_GetPort(j));
    UpnpSsdpJob_Delete(j);

    return true;
}

static CtRet parse_message(UpnpSsdp *thiz, const char *buf, uint32_t len, const char *ip, uint16_t port)
{
    HttpMessage *msg = NULL;
    CtRet ret = CT_RET_OK;

    do
    {
        HttpType type = HTTP_UNDEFINED;
        const char *method = NULL;
        const char *uri = NULL;

        msg = HttpMessage_New();
        if (msg == NULL)
        {
            ret = CT_RET_E_NEW;
            break;
        }

        ret = HttpMessage_Parse(msg, buf, len);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "parse message: invliad message");
            break;
        }

        switch (HttpMessage_GetType(msg))
        {
        case HTTP_REQUEST:
            if (HttpMessage_IsMethodEqual(msg, METHOD_NOTIFY))
            {
                parse_notify(thiz, msg, ip);
            }
            else if (HttpMessage_IsMethodEqual(msg, METHOD_MSEARCH))
            {
                parse_search_request(thiz, msg, ip, port);
            }
            else
            {
                LOG_D(TAG, "parse message: invliad HTTP method");
                break;
            }
            break;

        case HTTP_RESPONSE:
            parse_search_response(thiz, msg, ip, port);
            break;

        default:
            LOG_D(TAG, "parse message: invliad HTTP type");
            break;
        }
    }
    while (0);

    if (msg != NULL)
    {
        HttpMessage_Delete(msg);
    }

    return CT_RET_OK;
}

/*
NOTIFY * HTTP/1.1
Host:239.255.255.250:1900
NT:uuid:9ba32c90-9923-4ec6-81d0-335100229b91
NTS:ssdp:alive
Location:http://10.0.1.8:2869/upnphost/udhisapi.dll?content=uuid:9ba32c90-9923-4ec6-81d0-335100229b91
USN:uuid:9ba32c90-9923-4ec6-81d0-335100229b91
Cache-Control:max-age=1800
Server:Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0
OPT:"http://schemas.upnp.org/upnp/1/0/"; ns=01
01-NLS:f5662e2e617eb7532dbdedda7a40258a
*/

/*
NOTIFY * HTTP/1.1
Host:239.255.255.250:1900
NT:urn:schemas-upnp-org:service:ConnectionManager:1
NTS:ssdp:byebye
Location:http://10.0.1.8:2869/upnphost/udhisapi.dll?content=uuid:9ba32c90-9923-4ec6-81d0-335100229b91
USN:uuid:9ba32c90-9923-4ec6-81d0-335100229b91::urn:schemas-upnp-org:service:ConnectionManager:1
Cache-Control:max-age=1800
Server:Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0
OPT:"http://schemas.upnp.org/upnp/1/0/"; ns=01
01-NLS:f5662e2e617eb7532dbdedda7a40258a
*/
static void parse_notify(UpnpSsdp *thiz, HttpMessage *msg, const char *ip)
{
    UpnpObjectList_Lock(thiz->foundObjects);

    do
    {
        CtRet ret = CT_RET_OK;
        const char *usn = NULL;
        const char *nts = NULL;
        UpnpObject *obj = NULL;

        if (thiz->objectListener == NULL)
        {
            break;
        }

        usn = HttpMessage_GetHeaderValue(msg, HEAD_USN);
        if (usn == NULL)
        {
            break;
        }

        if (!validate_usn(thiz, usn))
        {
            break;
        }

        nts = HttpMessage_GetHeaderValue(msg, HEAD_NTS);
        if (nts == NULL)
        {
            break;
        }

        obj = UpnpObjectList_GetObject(thiz->foundObjects, usn);

        if (STR_EQUAL(nts, NTS_ALIVE))
        {
            if (obj != NULL)
            {
                UpnpObject_UpdateNextNotify(obj);
                break;
            }

            obj = convert_upnp_object(ip, msg, thiz->strict_uuid);
            if (obj == NULL)
            {
                break;
            }

            UpnpObjectList_AddObject(thiz->foundObjects, obj);
            thiz->objectListener(obj, true, thiz->ctx);
        }
        else if (STR_EQUAL(nts, NTS_BYEBYE))
        {
            if (obj == NULL)
            {
                break;
            }
                
            thiz->objectListener(obj, false, thiz->ctx);
            UpnpObjectList_RemoveObject(thiz->foundObjects, usn);
        }
    }
    while (0);

    UpnpObjectList_Unlock(thiz->foundObjects);

    return;
}

/*
M-SEARCH * HTTP/1.1
HOST: 239.255.255.250:1900
MAN: "ssdp:discover"
MX: 1
ST: urn:dial-multiscreen-org:service:dial:1
USER-AGENT: Google Chrome/38.0.2125.101 Linux
*/
static void parse_search_request(UpnpSsdp *thiz, HttpMessage *msg, const char *ip, uint16_t port)
{
    LOG_D(TAG, "parse message: M-SEARCH REQUEST (from: %s:%d)-> Not implemnted", ip, port);

    // TODO
}

/*
HTTP/1.1 200 OK
ST:urn:schemas-upnp-org:service:ConnectionManager:1
USN:uuid:aa4cd57c-0424-4545-a5de-a90d2bd78b12::urn:schemas-upnp-org:service:ConnectionManager:1
Location:http://10.0.1.8:2869/upnphost/udhisapi.dll?content=uuid:aa4cd57c-0424-4545-a5de-a90d2bd78b12
OPT:"http://schemas.upnp.org/upnp/1/0/"; ns=01
01-NLS:f5662e2e617eb7532dbdedda7a40258a
Cache-Control:max-age=900
Server:Microsoft-Windows-NT/5.1 UPnP/1.0 UPnP-Device-Host/1.0
Ext:
*/

static void parse_search_response(UpnpSsdp *thiz, HttpMessage *msg, const char *ip, uint16_t port)
{
    UpnpObjectList_Lock(thiz->foundObjects);

    do
    {
        CtRet ret = CT_RET_OK;
        const char *usn = NULL;
        UpnpObject *obj = NULL;

        if (thiz->objectListener == NULL)
        {
            break;
        }

        usn = HttpMessage_GetHeaderValue(msg, HEAD_USN);
        if (usn == NULL)
        {
            break;
        }

        if (! validate_usn(thiz, usn))
        {
            break;
        }

        obj = UpnpObjectList_GetObject(thiz->foundObjects, usn);
        if (obj != NULL)
        {
            UpnpObject_UpdateNextNotify(obj);
            break;
        }

        obj = convert_upnp_object(ip, msg, thiz->strict_uuid);
        if (obj != NULL)
        {
            UpnpObjectList_AddObject(thiz->foundObjects, obj);
            thiz->objectListener(obj, true, thiz->ctx);
        }
    } while (0);

    UpnpObjectList_Unlock(thiz->foundObjects);
}

static UpnpObject * convert_upnp_object(const char *ip, HttpMessage *msg, bool strict_uuid)
{
    UpnpObject *obj = NULL;

    do
    {
        CtRet ret = CT_RET_OK;
        const char *usn = NULL;
        const char *nt = NULL;
        const char *cache_control = NULL;
        const char *location = NULL;
        const char *server = NULL;

        usn = HttpMessage_GetHeaderValue(msg, HEAD_USN);
        if (usn == NULL)
        {
            break;
        }

        nt = HttpMessage_GetHeaderValue(msg, HEAD_NT);
        if (nt == NULL)
        {
            nt = HttpMessage_GetHeaderValue(msg, HEAD_ST);
            if (nt == NULL)
            {
                break;
            }
        }

        ret = validate_nt(nt, strict_uuid);
        if (RET_FAILED(ret))
        {
            break;
        }

        cache_control = HttpMessage_GetHeaderValue(msg, HEAD_CACHE_CONTROL);
        if (cache_control == NULL)
        {
            break;
        }

        location = HttpMessage_GetHeaderValue(msg, HEAD_LOCATION);
        if (location == NULL)
        {
            break;
        }

        server = HttpMessage_GetHeaderValue(msg, HEAD_SERVER);
        if (server == NULL)
        {
            break;
        }

        obj = UpnpObject_New();
        if (obj == NULL)
        {
            LOG_E(TAG, "UpnpObject_New failed");
            break;
        }

        UpnpObject_SetNt(obj, nt, strict_uuid);
        UpnpObject_SetCacheControl(obj, cache_control);
        UpnpObject_SetUsn(obj, usn);
        UpnpObject_SetIp(obj, ip);
        UpnpObject_SetLocation(obj, location);
        UpnpObject_SetStackInfo(obj, server);
        UpnpObject_UpdateNextNotify(obj);
    } while (0);

    return obj;
}

static bool validate_usn(UpnpSsdp *thiz, const char *str_usn)
{
    bool result = false;

    do
    {
        CtRet ret = CT_RET_OK;
        UpnpUsn usn;

        ret = UpnpUsn_Construct(&usn);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUsn_Parse(&usn, str_usn, thiz->strict_uuid);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpUsn_Parse failed: %s", ct_ret_to_str(ret));
            break;
        }

        result = (thiz->objectFilter == NULL) ? true : thiz->objectFilter(&usn, thiz->ctx);

        UpnpUsn_Dispose(&usn);
    } while (0);

    return result;
}

static CtRet validate_nt(const char *type, bool strict_uuid)
{
    CtRet ret = CT_RET_OK;

    do
    {
        UpnpUri uri;

        ret = UpnpUri_Construct(&uri);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = UpnpUri_Parse(&uri, type, strict_uuid);
        if (RET_FAILED(ret))
        {
            LOG_D(TAG, "UpnpUri_Parse failed: %s", ct_ret_to_str(ret));
        }

        UpnpUri_Dispose(&uri);

    } while (0);
    
    return ret;
}