/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   command.h
 *
 * @remark
 *
 */

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "sc_common.h"
#include "ScTimer.h"

SC_BEGIN_DECLS

bool on_timeout_listener(ScTimer *thiz, void *ctx);
void cmd_loop(ScTimer *timer);


SC_END_DECLS


#endif /* __COMMAND_H__ */
