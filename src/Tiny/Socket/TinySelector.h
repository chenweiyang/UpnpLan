/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   TinySelector.h
*
* @remark
*		set tabstop=4
*		set shiftwidth=4
*		set expandtab
*/

#ifndef __TINY_SELECTOR_H__
#define __TINY_SELECTOR_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


typedef struct _TinySelector
{
    uint32_t                ref;
    int                     max_fd;
    fd_set                  read_set;
    fd_set                  write_set;
} TinySelector;

typedef enum _TinySelectorOperation
{
    SELECTOR_OP_READ = 0,
    SELECTOR_OP_WRITE = 1,
} TinySelectorOperation;

typedef enum _TinySelectorRet
{
    SELECTOR_RET_OK = 0,
    SELECTOR_RET_ERROR = 1,
    SELECTOR_RET_TIMEOUT = 2,
} TinySelectorRet;

TinySelector * TinySelector_New(void);
TinyRet TinySelector_Construct(TinySelector *thiz);
TinyRet TinySelector_Dispose(TinySelector *thiz);
void TinySelector_Delete(TinySelector *thiz);

void TinySelector_Reset(TinySelector *thiz);
void TinySelector_Register(TinySelector *thiz, int fd, TinySelectorOperation op);
TinySelectorRet TinySelector_RunOnce(TinySelector *thiz, uint32_t ms);
bool TinySelector_IsReadable(TinySelector *thiz, int fd);
bool TinySelector_IsWriteable(TinySelector *thiz, int fd);


TINY_END_DECLS

#endif /* __TINY_SELECTOR_H__ */
