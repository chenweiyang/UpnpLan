/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpMessage.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#ifndef __HTTP_MESSAGE_H__
#define __HTTP_MESSAGE_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


typedef enum _HttpStatusCode
{
    /* info */
    HTTP_STATUS_CONTINUE                            = 100,
    HTTP_STATUS_SWITCHING_PROTOCOLS                 = 101,

    /* success */
    HTTP_STATUS_OK                                  = 200,
    HTTP_STATUS_CREATED                             = 201,
    HTTP_STATUS_ASCEPT                              = 202,
    HTTP_STATUS_NON_AUTH                            = 203,
    HTTP_STATUS_NO_CONTENT                          = 204,
    HTTP_STATUS_RESET_CONTENT                       = 205,
    HTTP_STATUS_PARTIAL_CONTENT                     = 206,

    /* redirect */
    HTTP_STATUS_MULTIPLE_CHOICES                    = 300,
    HTTP_STATUS_MOVED_PERMANENTLY                   = 301,
    HTTP_STATUS_FOUND                               = 302,
    HTTP_STATUS_SEE_OTHER                           = 303,
    HTTP_STATUS_NOT_MODIFIED                        = 304,
    HTTP_STATUS_USE_PROXY                           = 305,
    HTTP_STATUS_UNUSED_306                          = 306,
    HTTP_STATUS_TEMPORARY_REDIRECT                  = 307,

    /* client error */
    HTTP_STATUS_BAD_REQUEST                         = 400,
    HTTP_STATUS_UNAUTH                              = 401,
    HTTP_STATUS_PAYMENT_REQUIRED                    = 402,
    HTTP_STATUS_FORBIDDEN                           = 403,
    HTTP_STATUS_NOT_FOUND                           = 404,
    HTTP_STATUS_METHOD_NOT_ALLOWED                  = 405,
    HTTP_STATUS_NOT_ASCEPTABLE                      = 406,
    HTTP_STATUS_PROXY_AUTH_REQUIRED                 = 407,
    HTTP_STATUS_REQUEST_TIMEOUT                     = 408,
    HTTP_STATUS_CONFLICT                            = 409,
    HTTP_STATUS_GONE                                = 410,
    HTTP_STATUS_LENGTH_REQUIRED                     = 411,
    HTTP_STATUS_PRECODITION_FAILED                  = 412,
    HTTP_STATUS_REQUEST_ENTITY_TOO_LARGE            = 413,
    HTTP_STATUS_REQUEST_URI_TOOL_LONG               = 414,
    HTTP_STATUS_UNSUPPORTED_MEDIA_TYPE              = 415,
    HTTP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE     = 416,
    HTTP_STATUS_EXPECTATION_FAILED                  = 417,

    /* server error */
    HTTP_STATUS_INTERNAL_SERVER_ERROR               = 500,
    HTTP_STATUS_NOT_IMPLEMENTED                     = 501,
    HTTP_STATUS_BAD_GATEWAY                         = 502,
    HTTP_STATUS_SERVICE_UNAVAILABLE                 = 503,
    HTTP_STATUS_GATEWAY_TIMEOUT                     = 504,
    HTTP_STATUS_VERSION_NOT_SUPPORTED               = 505,
} HttpStatusCode;

typedef enum _HttpType
{
    HTTP_UNDEFINED  = 0,
    HTTP_REQUEST    = 1,
    HTTP_RESPONSE   = 2,
} HttpType;

#define PROTOCOL_HTTP       "HTTP"
#define PROTOCOL_RTSP       "RTSP"

struct _HttpMessage;
typedef struct _HttpMessage HttpMessage;

HttpMessage * HttpMessage_New(void);
TinyRet HttpMessage_Construct(HttpMessage *thiz);
TinyRet HttpMessage_Dispose(HttpMessage *thiz);
void HttpMessage_Delete(HttpMessage *thiz);
void HttpMessage_Copy(HttpMessage *dst, HttpMessage *src);

void HttpMessage_SetProtocolIdentifier(HttpMessage * thiz, const char *identifier);

void HttpMessage_SetIp(HttpMessage *thiz, const char *ip);
const char * HttpMessage_GetIp(HttpMessage *thiz);
void HttpMessage_SetPort(HttpMessage *thiz, uint16_t port);
uint16_t HttpMessage_GetPort(HttpMessage *thiz);

/* Parse bytes & to bytes */
TinyRet HttpMessage_Parse(HttpMessage * thiz, const char *bytes, uint32_t len);
TinyRet HttpMessage_ToBytes(HttpMessage *thiz, char **bytes, uint32_t *len);

void HttpMessage_SetType(HttpMessage * thiz, HttpType type);
HttpType HttpMessage_GetType(HttpMessage * thiz);

/* for request */
void HttpMessage_SetMethod(HttpMessage *thiz, const char * method);
void HttpMessage_SetUri(HttpMessage *thiz, const char * uri);
const char * HttpMessage_GetMethod(HttpMessage *thiz);
const char * HttpMessage_GetUri(HttpMessage *thiz);

/* for response */
void HttpMessage_SetResponse(HttpMessage *thiz, int code, const char *status);
const char * HttpMessage_GetStatus(HttpMessage *thiz);
int HttpMessage_GetStatusCode(HttpMessage *thiz);

/* for version */
void HttpMessage_SetVersion(HttpMessage *thiz, int major, int minor);
int HttpMessage_GetMajorVersion(HttpMessage *thiz);
int HttpMessage_GetMinorVersion(HttpMessage *thiz);

/* for method */
bool HttpMessage_IsMethodEqual(HttpMessage *thiz, const char *method);

/* for header */
void HttpMessage_SetHeader(HttpMessage *thiz, const char *name, const char *value);
void HttpMessage_SetHeaderInteger(HttpMessage *thiz, const char *name, uint32_t value);
const char * HttpMessage_GetHeaderValue(HttpMessage *thiz, const char *name);
uint32_t HttpMessage_GetHeaderCount(HttpMessage * thiz);
const char * HttpMessage_GetHeaderNameAt(HttpMessage * thiz, uint32_t index);
const char * HttpMessage_GetHeaderValueAt(HttpMessage * thiz, uint32_t index);

/* for content */
bool HttpMessage_IsContentFull(HttpMessage *thiz);
const char * HttpMessage_GetContentObject(HttpMessage *thiz);
uint32_t HttpMessage_GetContentSize(HttpMessage *thiz);
TinyRet HttpMessage_SetContentSize(HttpMessage *thiz, uint32_t size);
TinyRet HttpMessage_AddContentObject(HttpMessage *thiz, const char *bytes, uint32_t size);

/* set request */
TinyRet HttpMessage_SetRequest(HttpMessage *thiz, const char * method, const char *url);


TINY_END_DECLS

#endif /* __HTTP_MESSAGE_H__ */
