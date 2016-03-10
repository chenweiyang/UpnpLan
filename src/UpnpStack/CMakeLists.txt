#-----------------------------------------------------------------------------
#
# UpnpStack
#
#-----------------------------------------------------------------------------

#-----------------------
# UpnpCommon
#-----------------------
SET(UpnpCommon_Header
    UpnpCommon/upnp_api.h
    UpnpCommon/upnp_define.h)

SOURCE_GROUP(UpnpCommon\\headers        FILES     ${UpnpCommon_Header})

#-----------------------
# UpnpTypedef
#-----------------------
SET(UpnpTypedef_Header
    UpnpTypedef/ObjectType.h
    UpnpTypedef/ObjectValue.h
    UpnpTypedef/Object.h
    UpnpTypedef/PropertyDefinition.h
    UpnpTypedef/PropertyValue.h
    UpnpTypedef/Property.h
    UpnpTypedef/PropertyList.h
    UpnpTypedef/AllowedValueList.h
    UpnpTypedef/AllowedValueRange.h
	UpnpTypedef/UpnpActionDefinition.h
    UpnpTypedef/UpnpAction.h
    UpnpTypedef/UpnpActionList.h
    UpnpTypedef/UpnpState.h
    UpnpTypedef/UpnpStateList.h
    UpnpTypedef/UpnpServiceDefinition.h
    UpnpTypedef/UpnpService.h
    UpnpTypedef/UpnpServiceList.h
    UpnpTypedef/UpnpDeviceDefinition.h
    UpnpTypedef/UpnpDevice.h)

SET(UpnpTypedef_Source
    UpnpTypedef/ObjectType.c
    UpnpTypedef/Object.c
    UpnpTypedef/PropertyDefinition.c
    UpnpTypedef/PropertyValue.c
    UpnpTypedef/Property.c
    UpnpTypedef/PropertyList.c
    UpnpTypedef/AllowedValueList.c
    UpnpTypedef/AllowedValueRange.c
    UpnpTypedef/UpnpAction.c
    UpnpTypedef/UpnpActionList.c
    UpnpTypedef/UpnpState.c
    UpnpTypedef/UpnpStateList.c
    UpnpTypedef/UpnpService.c
    UpnpTypedef/UpnpServiceList.c
    UpnpTypedef/UpnpDevice.c)

SOURCE_GROUP(UpnpTypedef\\headers        FILES     ${UpnpTypedef_Header})
SOURCE_GROUP(UpnpTypedef\\sources        FILES     ${UpnpTypedef_Source})

#----------------------------------------------------------------------------
# source
#----------------------------------------------------------------------------
SET(SRC
    ${UpnpCommon_Header}
    ${UpnpTypedef_Header}
    ${UpnpTypedef_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(upnp_static STATIC ${SRC})
SET_TARGET_PROPERTIES(upnp_static PROPERTIES OUTPUT_NAME "upnp_static")
SET_TARGET_PROPERTIES(upnp_static PROPERTIES LINKER_LANGUAGE C)
ADD_DEFINITIONS(-DUPNP_STATIC)

#----------------------------------------------------------------------------
# shared library
#----------------------------------------------------------------------------
#ADD_LIBRARY(upnp_shared SHARED ${SRC})
#SET_TARGET_PROPERTIES(upnp_shared PROPERTIES OUTPUT_NAME "upnp")
#TARGET_LINK_LIBRARIES(upnp_shared expat tiny_static ${OS_LIB})

#----------------------------------------------------------------------------
# test
#----------------------------------------------------------------------------
#ADD_SUBDIRECTORY(test_UpnpCtrlPoint)
