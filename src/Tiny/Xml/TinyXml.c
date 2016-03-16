/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2014-9-12
*
* @file   TinyXml.c
*
* @remark
*
*/

#include "TinyXml.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "tiny_str_split.h"
#include <expat.h>

#define TAG                     "TinyXml"
#define XML_VERSION_LEN         8
#define XML_ENCODING_LEN        32

static void xml_start(void *userObject, const XML_Char *name, const XML_Char **atts);
static void xml_end(void *userObject, const XML_Char *name);
static void xml_data(void *userObject, const XML_Char *s, int len);
static void xml_cdata_start(void *userObject);
static void xml_cdata_end(void *userObject);

struct _TinyXml
{
    char            version[XML_VERSION_LEN];
    char            encoding[XML_ENCODING_LEN];
    TinyXmlNode   * node;
    bool            skip;
};

TinyXml * TinyXml_New(void)
{
    TinyXml *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinyXml *)tiny_malloc(sizeof(TinyXml));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinyXml_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinyXml_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet TinyXml_Construct(TinyXml *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinyXml));
        thiz->node = NULL;
    } while (0);

    return ret;
}

TinyRet TinyXml_Dispose(TinyXml *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    if (thiz->node != NULL)
    {
        TinyXmlNode_Dispose(thiz->node);
    }

    return TINY_RET_OK;
}

void TinyXml_Delete(TinyXml *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyXml_Dispose(thiz);
    tiny_free(thiz);
}

TinyRet TinyXml_Load(TinyXml *thiz, const char *file)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TinyRet TinyXml_Parse(TinyXml *thiz, const char *buffer, uint32_t length)
{
    LOG_TIME_BEGIN(TAG, TinyXml_Parse);
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        int is_final = 1;

        if (thiz->node != NULL)
        {
            ret = TINY_RET_E_STARTED;
            break;
        }

        XML_Parser parser = XML_ParserCreate(NULL);
        if (parser == NULL)
        {
            ret = TINY_RET_E_OUT_OF_MEMORY;
            break;
        }

        XML_SetElementHandler(parser, xml_start, xml_end);
        XML_SetCharacterDataHandler(parser, xml_data);
        XML_SetCdataSectionHandler(parser, xml_cdata_start, xml_cdata_end);
        XML_SetUserData(parser, thiz);

        do
        {
            if (XML_Parse(parser, buffer, length, is_final) == XML_STATUS_ERROR)
            {
                ret = TINY_RET_E_XML_INVALID;
                break;
            }
        } while (0);

        XML_ParserFree(parser);
    } while (0);

    LOG_TIME_END(TAG, TinyXml_Parse);
    return ret;
}

TinyRet TinyXml_ToString(TinyXml *thiz, char **string, uint32_t *length)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_E_NOT_IMPLEMENTED;
}

TinyXmlNode * TinyXml_GetRoot(TinyXml *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return thiz->node;
}

static void xml_start (void *userObject, const XML_Char *name, const XML_Char **atts)
{
    TinyXml * thiz = (TinyXml *)userObject;
    uint32_t i = 0;
    TinyXmlNode * node = NULL;
   
    node = TinyXmlNode_New();
    if (node == NULL)
    {
        LOG_E(TAG, "%s", tiny_ret_to_str(TINY_RET_E_OUT_OF_MEMORY));
        return;
    }

#if TINY_XML_DEBUG 
    printf("<%s", name);
#endif

    /**
    * usage:
    *     char *s = "urn:schemas-upnp-org:device:mediaserver:1"
    *     char group[10][128];
    *     uint32_t ret = str_split(s, ":", group, 10);
    *
    *     result:  (ret > 0)
    *       ret = 5;
    *       group[0] = "urn"
    *       group[1] = "schemas-upnp-org"
    *       group[2] = "device"
    *       group[3] = "mediaserver"
    *       group[4] = "1"
    */
    {
        char group[2][128];
        uint32_t ret = 0;

        memset(group, 0, 2 * 128);

        ret = str_split(name, ":", group, 2);
        if (ret == 1)
        {
            TinyXmlNode_SetName(node, name);
        }
        else if (ret == 2)
        {
            TinyXmlNode_SetNamePrefix(node, group[0]);
            TinyXmlNode_SetName(node, group[1]);
        }
    }

    for (i = 0; atts[i]; i ++)
    {
        const char * att_name = atts[i];
        const char * att_value = atts[i + 1];
        TinyXmlNode_AddAttribute(node, att_name, att_value);
        i++;

    #if TINY_XML_DEBUG 
        printf(" %s=%s", att_name, att_value);
    #endif
    }

#if TINY_XML_DEBUG 
    printf(">\n");
#endif

    if (thiz->node != NULL)
    {
        TinyXmlNode_AddChild(thiz->node, node);
    }

    thiz->node = node;

    /* <tag> begin, data must skip \r\n\t */
    thiz->skip = true;
}

static void xml_end(void *userObject, const XML_Char *name)
{
    TinyXml * thiz = (TinyXml *)userObject;
    if (TinyXmlNode_GetParent(thiz->node) != NULL)
    {
        thiz->node = TinyXmlNode_GetParent(thiz->node);
    }

    thiz->skip = true;

#if TINY_XML_DEBUG 
    printf("</%s>\n", name);
#endif
}

static void xml_data(void *userObject, const XML_Char *s, int len)
{
    TinyXml * thiz = (TinyXml *)userObject;
    int i = 0;
    const char *p = NULL;

    if (thiz->skip)
    {
        for (i = 0; i < len; i++)
        {
            /* skip SPACE and \r \n \t */
            if (s[i] == ' ' || s[i] == '\r' || s[i] == '\n' || s[i] == '\t')
            {
                continue;
            }

            break;
        }

        if (i == len)
        {
            return;
        }
    }

    thiz->skip = false;
    p = s + i;
    TinyXmlNode_SetText(thiz->node, p, len - i);
}

static void xml_cdata_start(void *userObject)
{
    //printf("xml_cdata_start\n");
}

static void xml_cdata_end(void *userObject)
{
    //printf("xml_cdata_end\n");
}
