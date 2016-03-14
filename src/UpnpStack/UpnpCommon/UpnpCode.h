/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   UpnpCode.h
*
* @remark
*      set tabstop=4
*      set shiftwidth=4
*      set expandtab
*/

#ifndef __UPNP_CODE_H__
#define __UPNP_CODE_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


typedef enum _UpnpCode {

    /* success */
    UPNP_SUCCESS = 200,

    /* The action name doest note mactch any of the actions for this service. */
    UPNP_ERR_INVALID_ACTION = 401,

    /* Not enough input arguments,
    or toomany input arguments,
    or input arguments with the wrong name,
    or input arguments with the wrong Object type. */
    UPNP_ERR_INVALID_ARGS = 402,

    /* The service doest not support the sate variable.  */
    UPNP_ERR_INVALID_VAR = 404,

    /* The current state of the service prevents invoking this action. */
    UPNP_ERR_ACTION_FAILED = 501,

    /* The argument value is invalid. */
    UPNP_ERR_ARGUMENT_VALUE_INVALID = 600,

    /* The argument value out of range. */
    UPNP_ERR_ARGUMENT_VALUE_OUT_OF_RANGE = 601,

    /* The requested action is optional and it has not been implemented .*/
    UPNP_ERR_OPTIONAL_ACTION_NOT_IMPLEMENTED = 602,

    /* The action can not be completed because the device does note have enough memroy. */
    UPNP_ERR_OUT_OF_MEMORY = 603,

    /* The action cannot be completed and human intervention is required. */
    UPNP_ERR_HUMAN_INTERVENTION_REQUIRED = 604,

    /* The action has a string argument that is too long. */
    UPNP_ERR_STRING_ARGUMENT_TOO_LONG = 605,

    /* The action requires authorization and the sender is not authorized. */
    UPNP_ERR_ACTION_NOT_AUTHORIZED = 606,

    UPNP_ERR_SIGNATURE_FAILURE = 607,
    UPNP_ERR_SIGNATURE_MISSING = 608,
    UPNP_ERR_NOT_ENCRYPTED = 609,
    UPNP_ERR_INVALID_SEQUENCE = 610,
    UPNP_ERR_INVALID_CONTROL_URL = 611,
    UPNP_ERR_NO_SUCH_SESSION = 612,
} UpnpCode;


TINY_END_DECLS

#endif /* __UPNP_CODE_H__ */
