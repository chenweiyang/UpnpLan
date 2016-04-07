/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   TinyMulticast.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "TinyMulticast.h"
#include "tiny_memory.h"
#include "tiny_log.h"
#include "tiny_net_util.h"

#define TAG         "TinyMulticast"

void OnSocketDelete (void * data, void *ctx)
{
    TinyMulticastSocket * s = (TinyMulticastSocket *)data;
    TinyMulticastSocket_Delete(s);
}

TinyMulticast * TinyMulticast_New(void)
{
    TinyMulticast *thiz = NULL;

    do
    {
        thiz = (TinyMulticast *)tiny_malloc(sizeof(TinyMulticast));
        if (thiz == NULL)
        {
            break;
        }

        if (RET_FAILED(TinyMulticast_Construct(thiz)))
        {
            TinyMulticast_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet TinyMulticast_Construct(TinyMulticast *thiz)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    do
    {
        memset(thiz, 0, sizeof(TinyMulticast));

        ret = TinyList_Construct(&thiz->list);
        if (RET_FAILED(ret))
        {
            LOG_E(TAG, "TinyList_Construct");
            break;
        }

        TinyList_SetDeleteListener(&thiz->list, OnSocketDelete, thiz);
    } while (0);

    return ret;
}

void TinyMulticast_Dispose(TinyMulticast *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMulticast_Close(thiz);
    TinyList_Dispose(&thiz->list);
    memset(thiz, 0, sizeof(TinyMulticast));
}

void TinyMulticast_Delete(TinyMulticast *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinyMulticast_Dispose(thiz);
    tiny_free(thiz);
}

static bool OnAddressOpen(const char *ip, void *ctx)
{
    TinyMulticast *thiz = (TinyMulticast *)ctx;

    do
    {
        uint32_t selfIp = tiny_net_ip_to_int(ip);;
        TinyMulticastSocket *s = NULL;
        
        s = TinyMulticastSocket_New();
        if (s == NULL)
        {
            LOG_E(TAG, "TinyMulticastSocket_New failed");
            break;
        }

        if (RET_FAILED(TinyMulticastSocket_Open(s, selfIp, thiz->group, thiz->port, thiz->block)))
        {
            LOG_E(TAG, "TinyMulticastSocket_Open failed");
            TinyMulticastSocket_Delete(s);
            break;
        }

        if (RET_FAILED(TinyList_AddTail(&thiz->list, s)))
        {
            LOG_E(TAG, "TinyList_AddTail failed");
            TinyMulticastSocket_Delete(s);
            break;
        }
    } while (0);

    return false;
}

TinyRet TinyMulticast_Open(TinyMulticast *thiz, const char *group, uint16_t port, bool block)
{
    TinyRet ret = TINY_RET_OK;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);
    RETURN_VAL_IF_FAIL(group, TINY_RET_E_ARG_NULL);

    do
    {
        if (TinyList_GetCount(&thiz->list) > 0)
        {
            LOG_E(TAG, "already opened");
            ret = TINY_RET_E_ITEM_EXIST;
            break;
        }

        strncpy(thiz->group, group, TINY_IP_LEN);
        thiz->port = port;
        thiz->block = block;

        tiny_net_for_each_ip(OnAddressOpen, thiz);
    } while (0);

    return ret;
}

TinyRet TinyMulticast_Close(TinyMulticast *thiz)
{
    int i = 0;

    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    for (i = 0; i < TinyList_GetCount(&thiz->list); ++i)
    {
        TinyMulticastSocket *s = (TinyMulticastSocket *)TinyList_GetAt(&thiz->list, i);
        TinyMulticastSocket_Close(s);
    }

    return TINY_RET_OK;
}

uint32_t TinyMulticast_GetCount(TinyMulticast *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, 0);

    return TinyList_GetCount(&thiz->list);
}

TinyMulticastSocket * TinyMulticast_GetSocketAt(TinyMulticast *thiz, uint32_t index)
{
    RETURN_VAL_IF_FAIL(thiz, NULL);

    return (TinyMulticastSocket *)TinyList_GetAt(&thiz->list, index);
}