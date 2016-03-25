/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   upnp_timeout_util.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "upnp_timeout_util.h"
#include "tiny_str_get_value.h"

#define TIMEOUT_INFINITE        "infinite"

TinyRet upnp_timeout_get_second(const char *timeout, uint32_t *second)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(timeout, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(second, TINY_RET_E_ARG_NULL);

    do
    {
        char v[16];
        int len = 0;

        memset(v, 0, 16);
        len = str_get_value(timeout, strlen(timeout), "Second-", NULL, v, 16);
        if (len <= 0)
        {
            ret = TINY_RET_E_ARG_INVALID;
            break;
        }

        *second = STR_EQUAL(v, TIMEOUT_INFINITE) ? 0 : atoi(v);
    } while (0);

    return ret;
}

TinyRet upnp_timeout_to_string(uint32_t second, char *string, uint32_t len)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(string, TINY_RET_E_ARG_NULL);

    if (second == 0)
    {
        tiny_snprintf(string, len, "Second-%s", TIMEOUT_INFINITE);
    }
    else
    {
        tiny_snprintf(string, len, "Second-%d", second);
    }

    return ret;
}