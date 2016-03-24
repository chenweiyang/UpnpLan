/**
 *
 * Copyright (C) 2007-2012
 *
 * @author jxengzi@gmail.com
 * @date   2010-5-25
 *
 * @file   tiny_md5.h
 *
 * @version 2010.5.25
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */


#ifndef __TINY_MD5_H__
#define __TINY_MD5_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


#define TINY_MD5_LEN    40

void tiny_md5_encode(const char* src, char result[TINY_MD5_LEN]);


TINY_END_DECLS

#endif /* __TINY_MD5_H__ */