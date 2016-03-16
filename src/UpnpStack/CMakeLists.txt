#-----------------------------------------------------------------------------
#
# UpnpStack
#
#-----------------------------------------------------------------------------

#-----------------------
# UpnpInitializer
#-----------------------
SET(UpnpInitializer_Header
    UpnpInitializer/UpnpInitializer.h
	)

SET(UpnpInitializer_Source
    UpnpInitializer/UpnpInitializer.c
	)

SOURCE_GROUP(UpnpInitializer\\headers        FILES     ${UpnpInitializer_Header})
SOURCE_GROUP(UpnpInitializer\\sources        FILES     ${UpnpInitializer_Source})

#-----------------------
# UpnpCommon
#-----------------------
SET(UpnpCommon_Header
    UpnpCommon/upnp_api.h
    UpnpCommon/upnp_define.h
    UpnpCommon/UpnpCode.h
    UpnpCommon/UpnpError.h
	)

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
    UpnpTypedef/UpnpDevice.h
	)

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
    UpnpTypedef/UpnpDevice.c
	)

SOURCE_GROUP(UpnpTypedef\\headers        FILES     ${UpnpTypedef_Header})
SOURCE_GROUP(UpnpTypedef\\sources        FILES     ${UpnpTypedef_Source})

#-----------------------
# Soap
#-----------------------
SET(UpnpSoap_Header
    UpnpSoap/SoapClient.h
    UpnpSoap/SoapMessage.h
    UpnpSoap/SoapDefinition.h
    )

SET(UpnpSoap_Source
    UpnpSoap/SoapClient.c
    UpnpSoap/SoapMessage.c
    )

SOURCE_GROUP(UpnpSoap\\headers        FILES     ${UpnpSoap_Header})
SOURCE_GROUP(UpnpSoap\\sources        FILES     ${UpnpSoap_Source})

#-----------------------
# UpnpRegistry
#-----------------------
SET(UpnpRegistry_Header
    UpnpRegistry/UpnpObject.h
    UpnpRegistry/UpnpObjectList.h
    UpnpRegistry/UpnpObjectMessage.h
    UpnpRegistry/UpnpRegistry.h
    UpnpRegistry/UpnpRegistryCore.h
    UpnpRegistry/UpnpUri.h
    UpnpRegistry/UpnpUsn.h
	)

SET(UpnpRegistry_Source
    UpnpRegistry/UpnpObject.c
    UpnpRegistry/UpnpObjectList.c
    UpnpRegistry/UpnpObjectMessage.c
    UpnpRegistry/UpnpRegistry.c
    UpnpRegistry/UpnpRegistryCore.c
    UpnpRegistry/UpnpUri.c
    UpnpRegistry/UpnpUsn.c
	)

SOURCE_GROUP(UpnpRegistry\\headers          FILES     ${UpnpRegistry_Header})
SOURCE_GROUP(UpnpRegistry\\sources          FILES     ${UpnpRegistry_Source})

#-----------------------
# UpnpCtrlPoint
#-----------------------
SET(UpnpCtrlPoint_Header
    UpnpCtrlPoint/UpnpEvent.h
	UpnpCtrlPoint/UpnpEventDefinition.h
	UpnpCtrlPoint/UpnpSubscriber.h
	UpnpCtrlPoint/UpnpSubscription.h
    UpnpCtrlPoint/UpnpActionInvoker.h
	UpnpCtrlPoint/UpnpSoapHelper.h
	)

SET(UpnpCtrlPoint_Source
    UpnpCtrlPoint/UpnpEvent.c
	UpnpCtrlPoint/UpnpSubscriber.c
	UpnpCtrlPoint/UpnpActionInvoker.c
    UpnpCtrlPoint/UpnpSoapHelper.c
	)

SOURCE_GROUP(UpnpCtrlPoint\\headers           FILES     ${UpnpCtrlPoint_Header})
SOURCE_GROUP(UpnpCtrlPoint\\sources           FILES     ${UpnpCtrlPoint_Source})

#-----------------------
# UpnpRuntime
#-----------------------
SET(UpnpRuntime_Header
    UpnpRuntime/UpnpRuntime.h
	UpnpRuntime/UpnpDeviceSummary.h
	UpnpRuntime/UpnpDeviceFactory.h
	UpnpRuntime/UpnpDeviceParser.h
	UpnpRuntime/UpnpServiceParser.h
	)

SET(UpnpRuntime_Source
    UpnpRuntime/UpnpRuntime.c
	UpnpRuntime/UpnpDeviceFactory.c
	UpnpRuntime/UpnpDeviceParser.c
	UpnpRuntime/UpnpServiceParser.c
	)

SOURCE_GROUP(UpnpRuntime\\headers           FILES     ${UpnpRuntime_Header})
SOURCE_GROUP(UpnpRuntime\\sources           FILES     ${UpnpRuntime_Source})

#----------------------------------------------------------------------------
# source
#----------------------------------------------------------------------------
SET(SRC
    ${UpnpInitializer_Header}
    ${UpnpInitializer_Source}
	${UpnpCommon_Header}
    ${UpnpTypedef_Header}
    ${UpnpTypedef_Source}
    ${UpnpSoap_Header}
    ${UpnpSoap_Source}
    ${UpnpRegistry_Header}
    ${UpnpRegistry_Source}
    ${UpnpCtrlPoint_Header}
    ${UpnpCtrlPoint_Source}
    ${UpnpRuntime_Header}
    ${UpnpRuntime_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
#ADD_LIBRARY(upnp_static STATIC ${SRC})
#SET_TARGET_PROPERTIES(upnp_static PROPERTIES OUTPUT_NAME "upnp_static")
#SET_TARGET_PROPERTIES(upnp_static PROPERTIES LINKER_LANGUAGE C)
#ADD_DEFINITIONS(-DUPNP_STATIC)

#----------------------------------------------------------------------------
# shared library
#----------------------------------------------------------------------------
ADD_LIBRARY(upnp_shared SHARED ${SRC})
SET_TARGET_PROPERTIES(upnp_shared PROPERTIES OUTPUT_NAME "upnp")
SET_TARGET_PROPERTIES(upnp_shared PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(upnp_shared tiny_static expat ${OS_LIB})

#----------------------------------------------------------------------------
# test
#----------------------------------------------------------------------------
ADD_SUBDIRECTORY(test_ControlPoint)
