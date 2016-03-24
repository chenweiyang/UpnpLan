/*
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_char_util.h
 *
 * @remark
 *		set tabstop=4
 *		set shiftwidth=4
 *		set expandtab
 */

#include "tiny_base.h"

#ifndef __TINY_CHAR_UTIL_H__
#define __TINY_CHAR_UTIL_H__


TINY_BEGIN_DECLS


int is_char(int c);
int is_ctl(int c);
int is_tspecial(int c);
int is_digit(int c);


TINY_END_DECLS

#endif /* __TINY_CHAR_UTIL_H__ */
