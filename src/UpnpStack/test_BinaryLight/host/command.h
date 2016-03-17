/*
 * Copyright (C) 2013-2015
 *
 * @author ouyangchengfeng@xiaomi.com
 * @date   2013-11-19
 *
 * @file   command.h
 *
 * @remark
 *
 */

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include <UpnpRuntime.h>
#include "BinaryLight.h"

TINY_BEGIN_DECLS

#define LOG(func, ret)  printf("%s: %s\n", func, tiny_ret_to_str(ret))

UpnpCode OnGetTarget(SwitchPower *thiz, SwitchPower_GetTargetResult *result, void *ctx);
UpnpCode OnSetTarget(SwitchPower *thiz, bool newTargetValue, void *ctx);
UpnpCode OnGetStatus(SwitchPower *thiz, SwitchPower_GetStatusResult *result, void *ctx);

void cmd_loop(UpnpRuntime * runtime, BinaryLight *light);


TINY_END_DECLS


#endif /* __COMMAND_H__ */
