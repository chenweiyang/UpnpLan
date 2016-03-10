/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   TinySocketIpc.h
*
* @remark
*
*/

#ifndef __TINY_SOCKET_IPC_H__
#define __TINY_SOCKET_IPC_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


#define MAX_IPC_MSG_LEN 1024

typedef enum _IpcMsgType
{
    IPC_MSG_STOP = 0,
    IPC_MSG_RESELECT = 1,
    IPC_MSG_USER_DEFINED = 2,
} IpcMsgType;

typedef struct _IpcMsg {
    IpcMsgType  type;
    char        msg[MAX_IPC_MSG_LEN];
} IpcMsg;

typedef struct _TinySocketIpc
{
    int         socket_fd;
    uint16_t    socket_port;
} TinySocketIpc;

TinySocketIpc * TinySocketIpc_New(void);
TinyRet TinySocketIpc_Construct(TinySocketIpc *thiz);
TinyRet TinySocketIpc_Dispose(TinySocketIpc *thiz);
void TinySocketIpc_Delete(TinySocketIpc *thiz);

int TinySocketIpc_GetFd(TinySocketIpc *thiz);
TinyRet TinySocketIpc_Send(TinySocketIpc *thiz, IpcMsg * ipc_msg);
TinyRet TinySocketIpc_Recv(TinySocketIpc *thiz, IpcMsg * ipc_msg);
TinyRet TinySocketIpc_SendStopMsg(TinySocketIpc *thiz);


TINY_END_DECLS

#endif /* __TINY_SOCKET_IPC_H__ */
