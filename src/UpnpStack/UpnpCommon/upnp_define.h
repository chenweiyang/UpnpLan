/*
* Copyright (C) 2013-2015
*
* @author jxfengzi@gmail.com
* @date   2013-7-9
*
* @file   upnp_define.h
*
* @remark
*      set tabstop4
*      set shiftwidth4
*      set expandtab
*/

#ifndef __UPNP_DEFINE_H__
#define __UPNP_DEFINE_H__

#include "tiny_base.h"

TINY_BEGIN_DECLS


#define UPNP_GROUP                              "239.255.255.250"
#define UPNP_PORT                               1900
#define UPNP_DDD_LEN                            (1024 * 20)
#define UPNP_SDD_LEN                            (1024 * 20)
#define UPNP_TIMEOUT                            (1000 * 5)
#define UPNP_SOAP_LEN                           (1024 * 20)
#define UPNP_STACK_INFO                         "Android/4.4 UPnP/1.0 UpnpLan/1.0"
#define UPNP_STACK_INFO_LEN                     128
#define UPNP_URI_LEN                            128
#define UPNP_USN_LEN                            128
#define UPNP_UUID_LEN                           68
#define UPNP_DOMAIN_NAME_LEN                    32
#define UPNP_TYPE_LEN                           32
#define UPNP_VERSION_LEN                        32
#define UPNP_ERR_DESCRIPTION_LEN                256

/* Schemas */
#define SCHEMAS_UPNP_ORG                        "schemas-upnp-org"

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

typedef struct _UpnpError
{
    int code;
    char description[UPNP_ERR_DESCRIPTION_LEN];
} UpnpError;


#if 0
/* UPnP Device */
#define DEVICE_MEDIA_RENDERER                   "MediaRenderer"
#define DEVICE_MEDIA_SERVER                     "MediaServer"

/* UPnP Service */
#define UPNP_AVT                                "AVTransport"
#define UPNP_CMS                                "ConnectionManager"
#define UPNP_CDS                                "ContentDirectory"
#define UPNP_RCS                                "RenderingControl"

/* AVT Actions*/
#define AVT_ACT_GET_CURRENT_TRANSPORT_ACTIONS   "GetCurrentTransportActions"
#define AVT_ACT_GET_DEVICE_CAPABILITIES         "GetDeviceCapabilities"
#define AVT_ACT_GET_MEDIA_INFO                  "GetMediaInfo"
#define AVT_ACT_GET_POSITION_INFO               "GetPositionInfo"
#define AVT_ACT_GET_TRANSPORT_INFO              "GetTransportInfo"
#define AVT_ACT_GET_TRANSPORT_SETTINGS          "GetTransportSettings"
#define AVT_ACT_NEXT                            "Next"
#define AVT_ACT_PAUSE                           "Pause"
#define AVT_ACT_PLAY                            "Play"
#define AVT_ACT_PREVIOUS                        "Previous"
#define AVT_ACT_SEEK                            "Seek"
#define AVT_ACT_SET_AVTRANSPORT_URI             "SetAVTransportURI"
#define AVT_ACT_SET_NEXT_AVTRANSPORT_URI        "SetNextAVTransportURI"
#define AVT_ACT_SET_PLAY_MODE                   "SetPlayMode"
#define AVT_ACT_STOP                            "Stop"

/* CMS Actions */
#define CMS_ACT_GET_CURRENT_CONNECTION_IDS      "GetCurrentConnectionIDs"
#define CMS_ACT_GET_CURRENT_CONNECTION_INFO     "GetCurrentConnectionInfo"
#define CMS_ACT_GET_PROTOCOL_INFO               "GetProtocolInfo"

/* RCS Actions */
#define RCS_ACT_GET_BRIGHTNESS                  "GetBrightness"
#define RCS_ACT_GET_CONTRAST                    "GetContrast"
#define RCS_ACT_GET_MUTE                        "GetMute"
#define RCS_ACT_GET_SHARPNESS                   "GetSharpness"
#define RCS_ACT_GET_VOLUME                      "GetVolume"
#define RCS_ACT_GET_VOLUME_DB                   "GetVolumeDB"
#define RCS_ACT_GET_VOLUME_DB_RANGE             "GetVolumeDBRange"
#define RCS_ACT_LIST_PRESETS                    "ListPresets"
#define RCS_ACT_SELECT_PRESET                   "SelectPreset"
#define RCS_ACT_SET_BRIGHTNESS                  "SetBrightness"
#define RCS_ACT_SET_MUTE                        "SetMute"
#define RCS_ACT_SET_SHARPNESS                   "SetSharpness"
#define RCS_ACT_SET_VOLUME                      "SetVolume"

/* CDS Actions */
#define CDS_ACT_BROWSE                          "Browse"
#define CDS_ACT_GET_SEARCH_CAPABILITIES         "GetSearchCapabilities"
#define CDS_ACT_GET_SORT_CAPABILITIES           "GetSortCapabilities"
#define CDS_ACT_GET_SYSTEM_UPDATE_ID            "GetSystemUpdateID"
#define CDS_ACT_SEARCH                          "Search"

/* AVT Argument */
#define AVT_ARG_INSTANCE_ID                     "InstanceID"
#define AVT_ARG_CURRENT_URI                     "CurrentURI"
#define AVT_ARG_CURRENT_URI_METAObject            "CurrentURIMetaObject"
#define AVT_ARG_SPEED                           "Speed"
#define AVT_ARG_UNIT                            "Unit"
#define AVT_ARG_TARGET                          "Target"
#define AVT_ARG_NEW_PLAY_MODE                   "NewPlayMode"

/* CMS Arguments */
#define CMS_ARG_CONNECTIONID                    "ConnectionID"

/* RCS Arguments */
#define RCS_ARG_INSTANCE_ID                     "InstanceID"
#define RCS_ARG_CHANNEL                         "Channel"
#define RCS_ARG_PRESET_NAME                     "PresetName"
#define RCS_ARG_DESIRED_BRIGHTNESS              "DesiredBrightness"
#define RCS_ARG_DESIRED_MUTE                    "DesiredMute"
#define RCS_ARG_DESIRED_SHARPNESS               "DesiredSharpness"
#define RCS_ARG_DESIRED_VOLUME                  "DesiredVolume"

/* CDS Arguments */
#define CDS_ARG_Object_ID                       "ObjectID"
#define CDS_ARG_BROWSER_FLAG                    "BrowseFlag"
#define CDS_ARG_FILTER                          "Filter"
#define CDS_ARG_STARTING_INDEX                  "StartingIndex"
#define CDS_ARG_REQUESTED_COUNT                 "RequestedCount"
#define CDS_ARG_SORT_CRITERIA                   "SortCriteria"
#define CDS_ARG_CONTAINER_ID                    "ContainerID"
#define CDS_ARG_SEARCH_CRITERIA                 "SearchCriteria"

/**
 * UPnP Error Codes - General
 */

/* The action name doest note mactch any of the actions for this service. */
#define UPNP_ERR_INVALID_ACTION                                 401

/* Not enough input arguments,
   or toomany input arguments,
   or input arguments with the wrong name,
   or input arguments with the wrong Object type. */
#define UPNP_ERR_INVALID_ARGS                                   402

/* The service doest not support the sate variable.  */
#define UPNP_ERR_INVALID_VAR                                    404

/* The current state of the service prevents invoking this action. */
#define UPNP_ERR_ACTION_FAILED                                  501

/* The argument value is invalid. */
#define UPNP_ERR_ARGUMENT_VALUE_INVALID                         600

/* The argument value out of range. */
#define UPNP_ERR_ARGUMENT_VALUE_OUT_OF_RANGE                    601

/* The requested action is optional and it has not been implemented .*/
#define UPNP_ERR_OPTIONAL_ACTION_NOT_IMPLEMENTED                602

/* The action can not be completed because the device does note have enough memroy. */
#define UPNP_ERR_OUT_OF_MEMORY                                  603

/* The action cannot be completed and human intervention is required. */
#define UPNP_ERR_HUMAN_INTERVENTION_REQUIRED                    604

/* The action has a string argument that is too long. */
#define UPNP_ERR_STRING_ARGUMENT_TOO_LONG                       605

/* The action requires authorization and the sender is not authorized. */
#define UPNP_ERR_ACTION_NOT_AUTHORIZED                          606

#define UPNP_ERR_SIGNATURE_FAILURE                              607
#define UPNP_ERR_SIGNATURE_MISSING                              608
#define UPNP_ERR_NOT_ENCRYPTED                                  609
#define UPNP_ERR_INVALID_SEQUENCE                               610
#define UPNP_ERR_INVALID_CONTROL_URL                            611
#define UPNP_ERR_NO_SUCH_SESSION                                612

/**
 * UPnP Error Codes - CDS
 */
#define UPNP_ERR_CDS_NO_SUCH_Object                             701
#define UPNP_ERR_CDS_UNSUPPORTED_OR_INVALID_SEARCH_CRITERIA     708
#define UPNP_ERR_CDS_UNSUPPORTED_OR_INVALID_SORT_CRITERIA       709
#define UPNP_ERR_CDS_NO_SUCH_CONTAINER                          710
#define UPNP_ERR_CDS_CANNOT_PROCESS_THE_REQUEST                 720

/**
 * UPnP Error Codes - CMS
 */
#define UPNP_ERR_CMS_INCOMPATIBLE_PROTOCOL_INFO                 701
#define UPNP_ERR_CMS_INCOMPATIBLE_DIRECTIONS                    702
#define UPNP_ERR_CMS_INSUFFICIENT_NEWWORK_RESOURCES             703
#define UPNP_ERR_CMS_LOCAL_RESTRICTIONS                         704
#define UPNP_ERR_CMS_ACCESS_DENIED                              705
#define UPNP_ERR_CMS_INVALID_CONNECTION_REFERENCE               706
#define UPNP_ERR_CMS_NOT_IN_NETWORK                             707

/**
 * UPnP Error Codes - AVT
 */
#define UPNP_ERR_AVT_TRANSITION_NOT_AVAILABLE                   701
#define UPNP_ERR_AVT_NO_CONTENTS                                702
#define UPNP_ERR_AVT_READ_ERROR                                 703
#define UPNP_ERR_AVT_TRANSPORT_IS_LOCKED                        705
#define UPNP_ERR_AVT_WRITE_ERROR                                706
#define UPNP_ERR_AVT_MEDIA_IS_PROTECTED_OR_NOT_WRITEABLE        707
#define UPNP_ERR_AVT_FORMAT_NOT_SUPPORTED_FOR_RECORDING         708
#define UPNP_ERR_AVT_MEDIA_IS_FULL                              709
#define UPNP_ERR_AVT_SEEK_MODE_NOT_SUPPORTED                    710
#define UPNP_ERR_AVT_ILLEGAL_SEEK_TARGET                        711
#define UPNP_ERR_AVT_PLAY_MODE_NOT_SUPPORTED                    712
#define UPNP_ERR_AVT_RECORD_QUALITY_OT_SUPPORTED                713
#define UPNP_ERR_AVT_ILLEGAL_MIME_TYPE                          714
#define UPNP_ERR_AVT_CONTENT_BUSY                               715
#define UPNP_ERR_AVT_RESOURCE_NOT_FOUND                         716
#define UPNP_ERR_AVT_PLAY_SPEED_NOT_SUPPORTED                   717
#define UPNP_ERR_AVT_INVALID_INSTANCE_ID                        718

/**
 * UPnP Error Codes - RCS
 */
#define UPNP_ERR_RCS_INVALID_NAME                               701
#define UPNP_ERR_RCS_INVALID_INSTANCE_ID                        702

/**
 * UPnP Media Class
 */
#define UPNP_MEDIA_IMAGE                                        "Object.item.imageItem"
#define UPNP_MEDIA_AUDIO                                        "Object.item.audioItem"
#define UPNP_MEDIA_VIDEO                                        "Object.item.videoItem"

/**
 * UpnpUriType
 */
typedef enum _UpnpUriType
{
    UPNP_URI_UNDEFINED          = 0,
    UPNP_URI_ROOT_DEVICE        = 1,
    UPNP_URI_UUID               = 2,
    UPNP_URI_UPNP_DEVICE        = 3,
    UPNP_URI_UPNP_SERVICE       = 4,
    UPNP_URI_NON_UPNP_DEVICE    = 5,
    UPNP_URI_NON_UPNP_SERVICE   = 6,
} UpnpUriType;

/**
 * UpnpObjectType
 */
typedef enum _UpnpObjectType
{
    UPNP_ObjectType_STRING  = 1,
    UPNP_DT_BOOL    = 2,
    UPNP_DT_UINT32  = 3,
    UPNP_DT_INT32   = 4,
    UPNP_DT_UINT16  = 5,
    UPNP_DT_INT16   = 6,
} UpnpObjectType;

/**
 * SpecVersion
 */
typedef struct _SpecVersion
{
    int major;
    int minor;
} SpecVersion;

typedef enum _UpnpServiceType
{
    UPNP_SERVICE_TYPE_UNKNOWN = 0,
    UPNP_SERVICE_TYPE_AVT = 1,
    UPNP_SERVICE_TYPE_CMS = 2,
    UPNP_SERVICE_TYPE_CDS = 3,
    UPNP_SERVICE_TYPE_RCS = 4,
} UpnpServiceType;

struct _UpnpAction;
typedef struct _UpnpAction UpnpAction;

struct _UpnpService;
typedef struct _UpnpService UpnpService;

struct _UpnpIcon;
typedef struct _UpnpIcon UpnpIcon;

struct _UpnpDevice;
typedef struct _UpnpDevice UpnpDevice;
#endif


TINY_END_DECLS

#endif /* __UPNP_DEFINE_H__ */
