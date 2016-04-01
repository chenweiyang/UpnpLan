/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-11-19
*
* @file   UpnpArgumentDirection.h
*
* @remark
*
*/

#include "UpnpArgumentDirection.h"

#define DIRECTION_UNKNOWN       "unknown"
#define DIRECTION_IN            "in"
#define DIRECTION_OUT           "out"

UpnpArgumentDirection UpnpArgumentDirection_Retrieve(const char *string)
{
    RETURN_VAL_IF_FAIL(string, ARG_UNKNOWN);

    if (STR_EQUAL(string, DIRECTION_IN))
    {
        return ARG_IN;
    }

    if (STR_EQUAL(string, DIRECTION_OUT))
    {
        return ARG_OUT;
    }

    return ARG_UNKNOWN;
}

const char * UpnpArgumentDirection_ToString(UpnpArgumentDirection direction)
{
    const char *string = NULL;
    
    RETURN_VAL_IF_FAIL(string, NULL);

    switch (direction)
    {
    case ARG_IN:
        string = DIRECTION_IN;
        break;

    case ARG_OUT:
        string = DIRECTION_OUT;
        break;

    default:
        string = DIRECTION_UNKNOWN;
        break;
    }

    return string;
}