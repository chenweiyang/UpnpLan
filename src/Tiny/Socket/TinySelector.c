/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   TinySelector.c
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#include "TinySelector.h"
#include "tiny_memory.h"
#include "tiny_log.h"

#define TAG     "TinySelector"

TinySelector * TinySelector_New(void)
{
    TinySelector *thiz = NULL;

    do
    {
        TinyRet ret = TINY_RET_OK;

        thiz = (TinySelector *)tiny_malloc(sizeof(TinySelector));
        if (thiz == NULL)
        {
            break;
        }

        ret = TinySelector_Construct(thiz);
        if (RET_FAILED(ret))
        {
            TinySelector_Delete(thiz);
            thiz = NULL;
            break;
        }
    } while (0);

    return thiz;
}

TinyRet TinySelector_Construct(TinySelector *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    memset(thiz, 0, sizeof(TinySelector));

    return TINY_RET_OK;
}

TinyRet TinySelector_Dispose(TinySelector *thiz)
{
    RETURN_VAL_IF_FAIL(thiz, TINY_RET_E_ARG_NULL);

    return TINY_RET_OK;
}

void TinySelector_Delete(TinySelector *thiz)
{
    RETURN_IF_FAIL(thiz);

    TinySelector_Dispose(thiz);
    tiny_free(thiz);
}

void TinySelector_Reset(TinySelector *thiz)
{
    RETURN_IF_FAIL(thiz);

    thiz->max_fd = 0;
    FD_ZERO(&thiz->read_set);
    FD_ZERO(&thiz->write_set);
}

void TinySelector_Register(TinySelector *thiz, int fd, TinySelectorOperation op)
{
    RETURN_IF_FAIL(thiz);

    if (op == SELECTOR_OP_READ)
    {
        FD_SET(fd, &thiz->read_set);
    }
    else
    {
        FD_SET(fd, &thiz->write_set);
    }

    /**
    * NOTE
    *   max_fd is not used on Windows,
    *   but on linux/unix it MUST Greater than socket_fd.
    */
    if (thiz->max_fd <= fd)
    {
        thiz->max_fd = fd + 1;
    }
}

TinySelectorRet TinySelector_RunOnce(TinySelector *thiz, uint32_t ms)
{
    TinySelectorRet result = SELECTOR_RET_OK;
    int ret = 0;

    RETURN_VAL_IF_FAIL(thiz, SELECTOR_RET_ERROR);

    if (ms == 0)
    {
        ret = select(thiz->max_fd, &thiz->read_set, &thiz->write_set, NULL, NULL);
    }
    else
    {
        struct timeval tv;
        tv.tv_sec = ms / 1000;
        tv.tv_usec = ms % 1000;

        ret = select(thiz->max_fd, &thiz->read_set, &thiz->write_set, NULL, &tv);
    }

    if (ret == 0)
    {
        result = SELECTOR_RET_TIMEOUT;
    }
    else if (ret < 0)
    {
        LOG_D(TAG, "select failed");
        result = SELECTOR_RET_ERROR;
    }
    else
    {
        result = SELECTOR_RET_OK;
    }

    return result;
}

bool TinySelector_IsReadable(TinySelector *thiz, int fd)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return FD_ISSET(fd, &thiz->read_set);
}

bool TinySelector_IsWriteable(TinySelector *thiz, int fd)
{
    RETURN_VAL_IF_FAIL(thiz, false);

    return FD_ISSET(fd, &thiz->write_set);
}
