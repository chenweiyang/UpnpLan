/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   SsdpMessage.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __SSDP_MESSAGE_H__
#define __SSDP_MESSAGE_H__

#include "tiny_base.h"
#include "UpnpDevice.h"
#include "UpnpService.h"

TINY_BEGIN_DECLS

/* method */
#define METHOD_NOTIFY               "NOTIFY"
#define METHOD_MSEARCH              "M-SEARCH"

/* headers */
#define HEAD_HOST                   "HOST"
#define HEAD_CACHE_CONTROL          "CACHE-CONTROL"
#define HEAD_LOCATION               "LOCATION"
#define HEAD_ST                     "ST"
#define HEAD_MAN                    "MAN"
#define HEAD_MX                     "MX"
#define HEAD_AL                     "AL"
#define HEAD_NT                     "NT"
#define HEAD_NTS                    "NTS"
#define HEAD_SERVER                 "SERVER"
#define HEAD_USN                    "USN"

/* header values max length */
#define HEAD_HOST_LEN               128
#define HEAD_CACHE_CONTROL_LEN      128
#define HEAD_LOCATION_LEN           256
#define HEAD_ST_LEN                 128
#define HEAD_MAN_LEN                64
#define HEAD_AL_LEN                 128
#define HEAD_NT_LEN                 128
#define HEAD_NTS_LEN                64
#define HEAD_SERVER_LEN             128
#define HEAD_USN_LEN                128

/* host value */
#define DEFAULT_HOST                "239.255.255.250:1900"

/* MAN value */
#define DEFAULT_MAN                 "\"ssdp:discover\""

/* ST value */
#define DEFAULT_ST                  "ssdp:all"

/* MX value */
#define DEFAULT_MX                  3

/* NTS value */
#define NTS_ALIVE                   "ssdp:alive"
#define NTS_BYEBYE                  "ssdp:byebye"

/* length */
#define SSDP_MSG_MAX_LEN            2048

/**
* Presence
* -------------------------------------------------------------------------------------
* <p> NOTIFY * HTTP/1.1 </p>
* <p> HOST: 239.255.255.250:1900 </p>
* <p> CACHE-CONTROL: max-age=[Message lifetime] </p>
* <p> LOCATION: [URL for the Device Description Document] </p>
* <p> AL: <[application-specific URI]><[application-specific URI]> </p>
* <p> NT: [URI indicating the type of presence advertisement] </p>
* <p> NTS: ssdp:alive </p>
* <p> SERVER: [OS and version] UPnP/1.0 </p>
* <p> USN: [URI identifying this message] </p>
*/
typedef struct _SsdpAlive
{
    char        host[HEAD_HOST_LEN + 1];
    char        nt[HEAD_NT_LEN + 1];
    char        nts[HEAD_NTS_LEN + 1];
    char        usn[HEAD_USN_LEN + 1];
    char        cache_control[HEAD_CACHE_CONTROL_LEN + 1];
    char        location[HEAD_LOCATION_LEN + 1];
    char        server[HEAD_SERVER_LEN + 1];
    char        ex_uri[TINY_URI_LEN];
    uint16_t    ex_port;
} SsdpAlive;

/**
* Exit Presence
* -------------------------------------------------------------------------------------
* <p> NOTIFY * HTTP/1.1 </p>
* <p> HOST: 239.255.255.250:1900 </p>
* <p> NT: [URI indicating the type of exit message] </p>
* <p> NTS: ssdp:byebye </p>
* <p> USN: [URI identifying this message] </p>
*/
typedef struct _SsdpByebye
{
    char        host[HEAD_HOST_LEN + 1];
    char        nt[HEAD_NT_LEN + 1];
    char        nts[HEAD_NTS_LEN + 1];
    char        usn[HEAD_USN_LEN + 1];
} SsdpByebye;

/**
* M-SEARCH request
* ----------------------------------------------
* <p> M-SEARCH * HTTP/1.1 </p>
* <p> HOST: 239.255.255.250:1900 </p>
* <p> ST: [URI for search target] </p>
* <p> MAN: "ssdp:discover" </p>
* <p> MX: [Maximum response delay in seconds] </p>
* ----------------------------------------------
*/
typedef struct _SsdpRequest
{
    char        host[HEAD_HOST_LEN + 1];
    char        st[HEAD_ST_LEN + 1];
    char        man[HEAD_MAN_LEN + 1];
    int         mx;
} SsdpRequest;

/**
* M-SEARCH response
* -------------------------------------------------------------------------------------
* <p> HTTP/1.1 200 OK </p>
* <p> CACHE-CONTROL: max-age=[Message lifetime] </p>
* <p> LOCATION: [URL for the Root Device Description] </p>
* <p> ST: [URI for Device and Service types] </p>
* <p> DATE: [date and time at the time of responding] </p>
* <p> SERVER: [OS and version] UPnP/1.0 </p>
* <p> USN: [URI identifying this message] </p>
*/
typedef struct _SsdpResponse
{
    char        cache_control[HEAD_CACHE_CONTROL_LEN + 1];
    char        location[HEAD_LOCATION_LEN + 1];
    char        st[HEAD_ST_LEN + 1];
    char        server[HEAD_SERVER_LEN + 1];
    char        usn[HEAD_USN_LEN + 1];
    char        ex_uri[TINY_URI_LEN];
    uint16_t    ex_port;
} SsdpResponse;

typedef enum _SsdpMessageType
{
    SSDP_INVALID = 0,
    SSDP_ALIVE = 1,
    SSDP_BYEBYE = 2,
    SSDP_MSEARCH_REQUEST = 3,
    SSDP_MSEARCH_RESPONSE = 4,
} SsdpMessageType;

typedef struct _SsdpMessage
{
    SsdpMessageType type;

    struct
    {
        char            ip[TINY_IP_LEN + 1];
        uint16_t        port;
    } remote;

    struct
    {
        char            ip[TINY_IP_LEN + 1];
    } local;

    union
    {
        SsdpAlive alive;
        SsdpByebye byebye;
        SsdpRequest request;
        SsdpResponse response;
    } v;
} SsdpMessage;

TinyRet SsdpMessage_Construct(SsdpMessage *thiz, const char *localIp, const char *remoteIp, uint16_t remotePort, const char *buf, uint32_t len);
TinyRet SsdpMessage_ConstructAlive_ROOTDEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *uri, uint16_t port);
TinyRet SsdpMessage_ConstructAlive_DEVICE_UUID(SsdpMessage *thiz, UpnpDevice *device, const char *uri, uint16_t port);
TinyRet SsdpMessage_ConstructAlive_DEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *uri, uint16_t port);
TinyRet SsdpMessage_ConstructAlive_SERVICE(SsdpMessage *thiz, UpnpService *service, const char *uri, uint16_t port);
TinyRet SsdpMessage_ConstructByebye_ROOTDEVICE(SsdpMessage *thiz, UpnpDevice *device);
TinyRet SsdpMessage_ConstructByebye_DEVICE_UUID(SsdpMessage *thiz, UpnpDevice *device);
TinyRet SsdpMessage_ConstructByebye_DEVICE(SsdpMessage *thiz, UpnpDevice *device);
TinyRet SsdpMessage_ConstructByebye_SERVICE(SsdpMessage *thiz, UpnpService *service);

TinyRet SsdpMessage_ConstructRequest(SsdpMessage *thiz, const char *target);
TinyRet SsdpMessage_ConstructResponse_ROOTDEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *uri, uint16_t ex_port, const char *localIp, const char *ip, uint16_t port);
TinyRet SsdpMessage_ConstructResponse_DEVICE_UUID(SsdpMessage *thiz, UpnpDevice *device, const char *uri, uint16_t ex_port, const char *localIp, const char *ip, uint16_t port);
TinyRet SsdpMessage_ConstructResponse_DEVICE(SsdpMessage *thiz, UpnpDevice *device, const char *uri, uint16_t ex_port, const char *localIp, const char *ip, uint16_t port);
TinyRet SsdpMessage_ConstructResponse_SERVICE(SsdpMessage *thiz, UpnpService *service, const char *uri, uint16_t ex_port, const char *localIp, const char *ip, uint16_t port);

void SsdpMessage_Dispose(SsdpMessage *thiz);

uint32_t SsdpMessage_ToString(SsdpMessage *thiz, char string[], uint32_t len);


TINY_END_DECLS

#endif /* __SSDP_MESSAGE_H__ */
