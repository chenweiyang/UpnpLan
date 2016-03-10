/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   HttpMessage.c
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "HttpMessage.h"
#include "tiny_char_util.h"
#include "tiny_url_split.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "HttpHeader.h"
#include "HttpContent.h"

#define TAG                 "HttpMessage"
#define CONTENT_LENGTH      "Content-Length"
#define PROTOCOL_LEN         8

/* HTTP/1.1 0 X */
#define HTTP_STATUS_LINE_MIN_LEN        14  /* strlen(HTTP/1.1 X 2\r\n) */
#define HTTP_REQUEST_LINE_MIN_LEN       14  /* X * HTTP/1.1\r\n */
#define HTTP_HEAD_LEN                   256
#define HTTP_LINE_LEN                   256
#define HTTP_METHOD_LEN                 32
#define HTTP_URI_LEN                    256
#define HTTP_STATUS_LEN                 256

static uint32_t HttpMessage_LoadStatusLine(HttpMessage * thiz, const char *bytes, uint32_t len);
static uint32_t HttpMessage_LoadRequestLine(HttpMessage * thiz, const char *bytes, uint32_t len);

typedef struct _HttpRequestLine
{
    char method[HTTP_METHOD_LEN];
    char uri[HTTP_URI_LEN];
} HttpRequestLine;

typedef struct _HttpStatusLine
{
    int code;
    char status[HTTP_STATUS_LEN];
} HttpStatusLine;

typedef struct _HttpVersion
{
    int major;
    int minor;
} HttpVersion;

struct _HttpMessage
{
    uint32_t            ref;
    HttpType            type;
    char                ip[CT_IP_LEN];
    uint16_t            port;
    char                protocol_identifier[PROTOCOL_LEN];

    HttpRequestLine     request_line;
    HttpStatusLine      status_line;
    HttpVersion         version;
    uint32_t            content_length;

    HttpHeader          header;
    HttpContent         content;
};

HttpMessage * HttpMessage_New(void)
{
    HttpMessage *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (HttpMessage *)tiny_malloc(sizeof(HttpMessage));
        if (thiz == NULL)
        {
            break;
        }

        ret = HttpMessage_Construct(thiz);
        if (RET_FAILED(ret))
        {
            HttpMessage_Delete(thiz);
            thiz = NULL;
            break;
        }
    }
    while (0);

    return thiz;
}

TinyRet HttpMessage_Construct(HttpMessage *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(HttpMessage));

        ret = HttpHeader_Construct(&thiz->header);
        if (RET_FAILED(ret))
        {
            break;
        }

        ret = HttpContent_Construct(&thiz->content);
        if (RET_FAILED(ret))
        {
            break;
        }

        strncpy(thiz->protocol_identifier, PROTOCOL_HTTP, PROTOCOL_LEN);
    }
    while (0);

    return ret;
}

TinyRet HttpMessage_Dispose(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    HttpContent_Dispose(&thiz->content);
    HttpHeader_Dispose(&thiz->header);

    return TINY_RET_OK;
}

void HttpMessage_Delete(HttpMessage *thiz)
{
    RETURN_IF_FAIL(thiz);

    HttpMessage_Dispose(thiz);
    tiny_free(thiz);
}

void HttpMessage_Copy(HttpMessage *dst, HttpMessage *src)
{
    RETURN_IF_FAIL(dst);
    RETURN_IF_FAIL(src);

    dst->ref = src->ref;
    dst->type = src->type;
    strncpy(dst->ip, src->ip, CT_IP_LEN);
    dst->port = src->port;
    strncpy(dst->protocol_identifier, src->protocol_identifier, PROTOCOL_LEN);
    strncpy(dst->request_line.method, src->request_line.method, HTTP_METHOD_LEN);
    strncpy(dst->request_line.uri, src->request_line.uri, HTTP_URI_LEN);
    dst->status_line.code = src->status_line.code;
    strncpy(dst->status_line.status, src->status_line.status, HTTP_STATUS_LEN);
    dst->version.major = src->version.major;
    dst->version.minor = src->version.minor;
    dst->content_length = src->content_length;
    HttpHeader_Copy(&dst->header, &src->header);
    HttpContent_Copy(&dst->content, &src->content);
}

void HttpMessage_SetProtocolIdentifier(HttpMessage * thiz, const char *identifier)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(identifier);

    strncpy(thiz->protocol_identifier, identifier, PROTOCOL_LEN);
}

void HttpMessage_SetIp(HttpMessage *thiz, const char *ip)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(ip);

    strncpy(thiz->ip, ip, CT_IP_LEN);
}

const char * HttpMessage_GetIp(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->ip;
}

void HttpMessage_SetPort(HttpMessage *thiz, uint16_t port)
{
    RETURN_IF_FAIL(thiz);

    thiz->port = port;
}

uint16_t HttpMessage_GetPort(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->port;
}

TinyRet HttpMessage_Parse(HttpMessage * thiz, const char *bytes, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t size = 0;
        const char * length = NULL;
        const char * data = bytes;
        uint32_t data_len = len;

        // load first line
        size = HttpMessage_LoadStatusLine(thiz, data, data_len);
        if (size == 0)
        {
            size = HttpMessage_LoadRequestLine(thiz, data, data_len);
            if (size == 0)
            {
                LOG_D(TAG, "HttpMessage_Parse => invalid first line");
                ret = TINY_RET_E_HTTP_MSG_INVALID;
                break;
            }
        }

        data += size;
        data_len -= size;

        // load headers
        size = HttpHeader_Parse(&thiz->header, data, data_len);
        if (size == 0)
        {
            LOG_D(TAG, "HttpMessage_Parse => invalid headers");
            ret = TINY_RET_E_HTTP_MSG_INVALID;
            break;
        }

        length = HttpHeader_GetValue(&thiz->header, CONTENT_LENGTH);
        thiz->content_length = (length != NULL) ? atoi(length) : 0;
        if (thiz->content_length == 0)
        {
            break;
        }

        data += size;
        data_len -= size;

        // skip '\n'
        if (data_len > 0)
        {
            data++;
            data_len--;
        }

        ret = HttpContent_SetSize(&thiz->content, thiz->content_length);
        if (RET_FAILED(ret))
        {
            break;
        }

        // load content
        if (data_len > 0)
        {
            HttpContent_AddObject(&thiz->content, data, data_len);
        }

    } while (0);
    
    return ret;
}

TinyRet HttpMessage_ToBytes(HttpMessage *thiz, char **bytes, uint32_t *len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        uint32_t i = 0;
        uint32_t count = 0;
        uint32_t content_length = 0;
        uint32_t buffer_size = 0;
        char *p = NULL;
        char line[HTTP_LINE_LEN];

        if (thiz->type == HTTP_UNDEFINED)
        {
            ret = TINY_RET_E_HTTP_TYPE_INVALID;
            break;
        }

        // calculate size
        count = HttpHeader_GetCount(&thiz->header);
        content_length = HttpContent_GetSize(&thiz->content);
        buffer_size = HTTP_LINE_LEN + count * HTTP_HEAD_LEN + content_length;

        *bytes = (char *)tiny_malloc(buffer_size);
        if (*bytes == NULL)
        {
            ret = TINY_RET_E_NEW;
            break;
        }

        memset(*bytes, 0, buffer_size);
        p = *bytes;

        // first line
        memset(line, 0, HTTP_LINE_LEN);
        if (thiz->type == HTTP_REQUEST)
        {
            tiny_snprintf(line,
                HTTP_LINE_LEN,
                "%s %s %s/%d.%d\r\n",
                thiz->request_line.method,
                thiz->request_line.uri,
                thiz->protocol_identifier,
                thiz->version.major,
                thiz->version.minor);
        }
        else
        {
            // RESPONSE
            tiny_snprintf(line,
                HTTP_LINE_LEN,
                "%s/%d.%d %d %s\r\n",
                thiz->protocol_identifier,
                thiz->version.major,
                thiz->version.minor,
                thiz->status_line.code,
                thiz->status_line.status);
        }
        line[HTTP_LINE_LEN - 1] = 0;

        strncpy(p, line, buffer_size);
        p += strlen(line);

        // headers
        for (i = 0; i < count; ++i)
        {
            const char * name = HttpHeader_GetNameAt(&thiz->header, i);
            const char * value = HttpHeader_GetValueAt(&thiz->header, i);

            memset(line, 0, HTTP_LINE_LEN);
            tiny_snprintf(line, HTTP_LINE_LEN, "%s: %s\r\n", name, value);
            line[HTTP_LINE_LEN - 1] = 0;

            strncpy(p, line, HTTP_LINE_LEN);
            p += strlen(line);
        }

        // \r\n
        memset(line, 0, HTTP_LINE_LEN);
        tiny_snprintf(line, HTTP_LINE_LEN, "\r\n");
        line[HTTP_LINE_LEN - 1] = 0;

        strncpy(p, line, HTTP_LINE_LEN);
        p += strlen(line);

        // content
        if (content_length > 0)
        {
            memcpy(p, HttpContent_GetObject(&thiz->content), content_length);
            p += content_length;
        }

        *len = p - *bytes;
    } while (0);

    return ret;
}

void HttpMessage_SetType(HttpMessage * thiz, HttpType type)
{
    RETURN_IF_FAIL(thiz);

    thiz->type = type;
}

HttpType HttpMessage_GetType(HttpMessage * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, HTTP_UNDEFINED);

    return thiz->type;
}

void HttpMessage_SetMethod(HttpMessage *thiz, const char * method)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(method);

    strncpy(thiz->request_line.method, method, HTTP_METHOD_LEN);
}

void HttpMessage_SetUri(HttpMessage *thiz, const char * uri)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(uri);
    
    strncpy(thiz->request_line.uri, uri, HTTP_URI_LEN);
}

const char * HttpMessage_GetMethod(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->request_line.method;
}

const char * HttpMessage_GetUri(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->request_line.uri;
}

void HttpMessage_SetResponse(HttpMessage *thiz, int code, const char *status)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(status);

    thiz->status_line.code = code;
    strncpy(thiz->status_line.status, status, HTTP_STATUS_LEN);
}

const char * HttpMessage_GetStatus(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->status_line.status;
}

int HttpMessage_GetStatusCode(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->status_line.code;
}

void HttpMessage_SetVersion(HttpMessage *thiz, int major, int minor)
{
    RETURN_IF_FAIL(thiz);

    thiz->version.major = major;
    thiz->version.minor = minor;
}

int HttpMessage_GetMajorVersion(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->version.major; 
}

int HttpMessage_GetMinorVersion(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return thiz->version.minor;
}

HttpHeader * HttpMessage_GetHeader(HttpMessage * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return &thiz->header;
}

HttpContent * HttpMessage_GetContent(HttpMessage * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return &thiz->content;
}

void HttpMessage_SetHeader(HttpMessage * thiz, const char *name, const char *value)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);
    RETURN_IF_FAIL(value);

    HttpHeader_Set(&thiz->header, name, value);
}

void HttpMessage_SetHeaderInteger(HttpMessage * thiz, const char *name, uint32_t value)
{
    RETURN_IF_FAIL(thiz);
    RETURN_IF_FAIL(name);
    RETURN_IF_FAIL(value);

    HttpHeader_SetInteger(&thiz->header, name, value);
}

const char * HttpMessage_GetHeaderValue(HttpMessage * thiz, const char *name)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);
    RETURN_VAL_IF_FAIL(name, NULL);

    return HttpHeader_GetValue(&thiz->header, name);
}

uint32_t HttpMessage_GetHeaderCount(HttpMessage * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return HttpHeader_GetCount(&thiz->header);
}

const char * HttpMessage_GetHeaderNameAt(HttpMessage * thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return HttpHeader_GetNameAt(&thiz->header, index);
}

const char * HttpMessage_GetHeaderValueAt(HttpMessage * thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return HttpHeader_GetValueAt(&thiz->header, index);
}

const char * HttpMessage_GetContentObject(HttpMessage * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return HttpContent_GetObject(&thiz->content);
}

uint32_t HttpMessage_GetContentSize(HttpMessage * thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return HttpContent_GetSize(&thiz->content);
}

bool HttpMessage_IsMethodEqual(HttpMessage * thiz, const char *method)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return STR_EQUAL(thiz->request_line.method, method);
}

bool HttpMessage_IsContentFull(HttpMessage *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return HttpContent_IsFull(&thiz->content);
}

TinyRet HttpMessage_SetContentSize(HttpMessage *thiz, uint32_t size)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return HttpContent_SetSize(&thiz->content, size);
}

TinyRet HttpMessage_AddContentObject(HttpMessage *thiz, const char *bytes, uint32_t size)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return HttpContent_AddObject(&thiz->content, bytes, size);
}

static uint32_t HttpMessage_LoadStatusLine(HttpMessage * thiz, const char *bytes, uint32_t len)
{
    int i = 0;
    int count = 0;
    const char *p = bytes;

    if (len < HTTP_STATUS_LINE_MIN_LEN)
    {
        return 0;
    }

    // protocol identifier.
    count = strlen(thiz->protocol_identifier);
    for (i = 0; i < count; ++i)
    {
        if (p[i] != thiz->protocol_identifier[i])
        {
            return 0;
        }
    }

    p += count;

    // Slash.
    if (*p++ != '/')
    {
        return 0;
    }

    // Major version number.
    if (!is_digit(*p))
    {
        return 0;
    }

    thiz->version.major = 0;
    while (is_digit(*p))
    {
        thiz->version.major = thiz->version.major * 10 + *p - '0';
        p++;
    }

    // Dot.
    if (*p++ != '.')
    {
        return 0;
    }

    // Minor version number.
    if (!is_digit(*p))
    {
        return 0;
    }

    thiz->version.minor = 0;
    while (is_digit(*p))
    {
        thiz->version.minor = thiz->version.minor * 10 + *p - '0';
        p++;
    }

    // Space.
    if (*p++ != ' ')
    {
        return 0;
    }

    // status code
    thiz->status_line.code = 0;
    while (is_digit(*p))
    {
        thiz->status_line.code = thiz->status_line.code * 10 + *p - '0';
        p++;
    }

    // Space.
    if (*p++ != ' ')
    {
        return 0;
    }

    // status
    memset(thiz->status_line.status, 0, HTTP_STATUS_LEN);
    while (!is_ctl(*p))
    {
        thiz->status_line.status[i++] = *p;
        p++;
    }

    // CRLF.
    if (*p++ != '\r')
    {
        return 0;
    }

    if (*p++ != '\n')
    {
        return 0;
    }

    thiz->type = HTTP_RESPONSE;

    // length of status line
    return (p - bytes);
}

static uint32_t HttpMessage_LoadRequestLine(HttpMessage * thiz, const char *bytes, uint32_t len)
{
    int i = 0;
    int count = 0;
    const char *p = bytes;

    if (len < HTTP_STATUS_LINE_MIN_LEN)
    {
        return 0;
    }

    // Request method.
    memset(thiz->request_line.method, 0, HTTP_METHOD_LEN);
    i = 0;
    while (is_char(*p) && !is_ctl(*p) && !is_tspecial(*p) && *p != ' ')
    {
        thiz->request_line.method[i++] = *p;
        p++;
    }

    if (strlen(thiz->request_line.method) == 0)
    {
        return 0;
    }

    // Space.
    if (*p++ != ' ')
    {
        return 0;
    }

    // URI.
    memset(thiz->request_line.uri, 0, HTTP_URI_LEN);
    i = 0;
    while (!is_ctl(*p) && *p != ' ')
    {
        thiz->request_line.uri[i++] = *p;
        p++;
    }

    if (strlen(thiz->request_line.uri) == 0)
    {
        return 0;
    }

    // Space.
    if (*p++ != ' ')
    {
        return 0;
    }

    // protocol identifier.
    count = strlen(thiz->protocol_identifier);
    for (i = 0; i < count; ++i)
    {
        if (p[i] != thiz->protocol_identifier[i])
        {
            return 0;
        }
    }
    p += count;

    // Slash.
    if (*p++ != '/')
    {
        return 0;
    }

    // Major version number.
    if (! is_digit(*p)) 
    {
        return 0;
    }

    thiz->version.major = 0;
    while (is_digit(*p))
    {
        thiz->version.major = thiz->version.major * 10 + *p - '0';
        p++;
    }

    // Dot.
    if (*p++ != '.')
    {
        return 0;
    }

    // Minor version number.
    if (!is_digit(*p))
    {
        return 0;
    }

    thiz->version.minor = 0;
    while (is_digit(*p))
    {
        thiz->version.minor = thiz->version.minor * 10 + *p - '0';
        p++;
    }

    // CRLF.
    if (*p++ != '\r')
    {
        return 0;
    }

    if (*p++ != '\n')
    {
        return 0;
    }

    thiz->type = HTTP_REQUEST;

    // length of status line
    return (p - bytes);
}

TinyRet HttpMessage_SetRequest(HttpMessage *thiz, const char * method, const char *url)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(method, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(url, TINY_RET_E_ARG_NULL);

    do
    {
        char ip[CT_IP_LEN];
        uint16_t port = 0;
        char uri[512];
        char host[128];
        
        memset(ip, 0, CT_IP_LEN);
        memset(uri, 0, 512);
        memset(host, 0, 128);

        ret = url_split(url, ip, CT_IP_LEN, &port, uri, 512);
        if (RET_FAILED(ret))
        {
            LOG_W(TAG, "url_split: %s", tiny_ret_to_str(ret));
            break;
        }

        if (port == 80)
        {
            tiny_snprintf(host, 128, "%s", ip);
        }
        else
        {
            tiny_snprintf(host, 128, "%s:%d", ip, port);
        }

        HttpMessage_SetIp(thiz, ip);
        HttpMessage_SetPort(thiz, port);
        HttpMessage_SetType(thiz, HTTP_REQUEST);
        HttpMessage_SetVersion(thiz, 1, 1);
        HttpMessage_SetMethod(thiz, method);
        HttpMessage_SetUri(thiz, uri);
        HttpHeader_Set(&thiz->header, "HOST", host);
    } while (0);

    return ret;
}
