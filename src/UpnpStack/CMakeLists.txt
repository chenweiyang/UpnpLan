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
    UpnpCommon/upnp_net_util.h
    UpnpCommon/upnp_timeout_util.h
    )

SET(UpnpCommon_Source
    UpnpCommon/upnp_net_util.c
    UpnpCommon/upnp_timeout_util.c
    )

SOURCE_GROUP(UpnpCommon\\headers        FILES     ${UpnpCommon_Header})
SOURCE_GROUP(UpnpCommon\\Source         FILES     ${UpnpCommon_Source})

#-----------------------
# UpnpTypedef
#-----------------------
SET(UpnpTypedef_Header
    UpnpTypedef/UpnpListener.h
    UpnpTypedef/Property.h
    UpnpTypedef/PropertyList.h
    UpnpTypedef/AllowedValueList.h
    UpnpTypedef/AllowedValueRange.h
    UpnpTypedef/UpnpAction.h
    UpnpTypedef/UpnpArgument.h
    UpnpTypedef/UpnpArgumentDirection.h
    UpnpTypedef/UpnpStateVariable.h
    UpnpTypedef/UpnpStateVariableDefinition.h
    UpnpTypedef/InternalData.h
    UpnpTypedef/DataType.h
    UpnpTypedef/DataValue.h
    UpnpTypedef/UpnpService.h
    UpnpTypedef/UpnpDevice.h
    UpnpTypedef/UpnpDeviceSummary.h
    UpnpTypedef/UpnpUri.h
    UpnpTypedef/UpnpUsn.h
    UpnpTypedef/UpnpEvent.h
    UpnpTypedef/UpnpSubscriber.h
    )

SET(UpnpTypedef_Source
    UpnpTypedef/Property.c
    UpnpTypedef/PropertyList.c
    UpnpTypedef/AllowedValueList.c
    UpnpTypedef/AllowedValueRange.c
    UpnpTypedef/DataType.c
    UpnpTypedef/DataValue.c
    UpnpTypedef/UpnpArgument.c
    UpnpTypedef/UpnpArgumentDirection.c
    UpnpTypedef/UpnpAction.c
    UpnpTypedef/UpnpStateVariable.c
    UpnpTypedef/UpnpStateVariableDefinition.c
    UpnpTypedef/UpnpService.c
    UpnpTypedef/UpnpDevice.c
    UpnpTypedef/UpnpUri.c
    UpnpTypedef/UpnpUsn.c
    UpnpTypedef/UpnpEvent.c
    UpnpTypedef/UpnpSubscriber.c
    )

SOURCE_GROUP(UpnpTypedef\\headers        FILES     ${UpnpTypedef_Header})
SOURCE_GROUP(UpnpTypedef\\sources        FILES     ${UpnpTypedef_Source})

#-----------------------
# UpnpHttp
#-----------------------
SET(UpnpHttp_Header
    UpnpHttp/message/soap/SoapMessage.h
    UpnpHttp/message/ActionRequest.h
    UpnpHttp/message/ActionResponse.h
    UpnpHttp/message/ServiceSubRequest.h
    UpnpHttp/message/ServiceSubResponse.h
    UpnpHttp/message/ServiceUnsubRequest.h
    UpnpHttp/message/ServiceUnsubResponse.h
    UpnpHttp/message/EventRequest.h
    UpnpHttp/UpnpHttpManager.h
    UpnpHttp/UpnpHttpClient.h
    UpnpHttp/UpnpHttpConnection.h
    UpnpHttp/UpnpHttpServer.h
    )

SET(UpnpHttp_Source
    UpnpHttp/message/soap/SoapMessage.c
    UpnpHttp/message/ActionRequest.c
    UpnpHttp/message/ActionResponse.c
    UpnpHttp/message/ServiceSubRequest.c
    UpnpHttp/message/ServiceSubResponse.c
    UpnpHttp/message/ServiceUnsubRequest.c
    UpnpHttp/message/ServiceUnsubResponse.c
    UpnpHttp/message/EventRequest.c
    UpnpHttp/UpnpHttpManager.c
    UpnpHttp/UpnpHttpClient.c
    UpnpHttp/UpnpHttpConnection.c
    UpnpHttp/UpnpHttpServer.c
    )

SOURCE_GROUP(UpnpHttp\\headers        FILES     ${UpnpHttp_Header})
SOURCE_GROUP(UpnpHttp\\sources        FILES     ${UpnpHttp_Source})

#-----------------------
# UpnpProvider
#-----------------------
SET(UpnpProvider_Header
    UpnpProvider/UpnpProvider.h
    UpnpProvider/UpnpObserver.h
    )

SET(UpnpProvider_Source
    UpnpProvider/UpnpProvider.c
    UpnpProvider/UpnpObserver.c
    )

SOURCE_GROUP(UpnpProvider\\headers           FILES     ${UpnpProvider_Header})
SOURCE_GROUP(UpnpProvider\\sources           FILES     ${UpnpProvider_Source})

#-----------------------
# UpnpRegistry
#-----------------------
SET(UpnpRegistry_Header
    UpnpRegistry/UpnpObject.h
    UpnpRegistry/UpnpObjectList.h
    UpnpRegistry/UpnpObjectMessage.h
    UpnpRegistry/UpnpRegistry.h
    UpnpRegistry/UpnpValidator.h
    UpnpRegistry/UpnpObjectFactory.h
    UpnpRegistry/Ssdp.h
    UpnpRegistry/SsdpMessage.h
    )

SET(UpnpRegistry_Source
    UpnpRegistry/UpnpObject.c
    UpnpRegistry/UpnpObjectList.c
    UpnpRegistry/UpnpObjectMessage.c
    UpnpRegistry/UpnpRegistry.c
    UpnpRegistry/UpnpValidator.c
    UpnpRegistry/UpnpObjectFactory.c
    UpnpRegistry/Ssdp.c
    UpnpRegistry/SsdpMessage.c
    )

SOURCE_GROUP(UpnpRegistry\\headers          FILES     ${UpnpRegistry_Header})
SOURCE_GROUP(UpnpRegistry\\sources          FILES     ${UpnpRegistry_Source})

#-----------------------
# UpnpCtrlPoint
#-----------------------
SET(UpnpCtrlPoint_Header
    UpnpCtrlPoint/UpnpGenaClient.h
    UpnpCtrlPoint/UpnpSubscription.h
    UpnpCtrlPoint/UpnpActionInvoker.h
    )

SET(UpnpCtrlPoint_Source
    UpnpCtrlPoint/UpnpGenaClient.c
    UpnpCtrlPoint/UpnpSubscription.c
    UpnpCtrlPoint/UpnpActionInvoker.c
    )

SOURCE_GROUP(UpnpCtrlPoint\\headers           FILES     ${UpnpCtrlPoint_Header})
SOURCE_GROUP(UpnpCtrlPoint\\sources           FILES     ${UpnpCtrlPoint_Source})

#-----------------------
# UpnpHost
#-----------------------
SET(UpnpHost_Header
    UpnpHost/UpnpHost.h
    UpnpHost/UpnpActionExecutor.h
    UpnpHost/UpnpDocumentGetter.h
    UpnpHost/UpnpGenaServer.h
    )

SET(UpnpHost_Source
    UpnpHost/UpnpHost.c
    UpnpHost/UpnpActionExecutor.c
    UpnpHost/UpnpDocumentGetter.c
    UpnpHost/UpnpGenaServer.c
    )

SOURCE_GROUP(UpnpHost\\headers               FILES     ${UpnpHost_Header})
SOURCE_GROUP(UpnpHost\\sources               FILES     ${UpnpHost_Source})

#-----------------------
# UpnpRuntime
#-----------------------
SET(UpnpRuntime_Header
    UpnpRuntime/UpnpRuntime.h
    UpnpRuntime/UpnpDeviceConfig.h
    UpnpRuntime/UpnpDeviceFactory.h
    UpnpRuntime/UpnpDeviceParser.h
    UpnpRuntime/UpnpServiceParser.h
    UpnpRuntime/UpnpServiceHelper.h
    )

SET(UpnpRuntime_Source
    UpnpRuntime/UpnpRuntime.c
    UpnpRuntime/UpnpDeviceConfig.c
    UpnpRuntime/UpnpDeviceFactory.c
    UpnpRuntime/UpnpDeviceParser.c
    UpnpRuntime/UpnpServiceParser.c
    UpnpRuntime/UpnpServiceHelper.c
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
    ${UpnpCommon_Source}
    ${UpnpTypedef_Header}
    ${UpnpTypedef_Source}
    ${UpnpHttp_Header}
    ${UpnpHttp_Source}
    ${UpnpProvider_Header}
    ${UpnpProvider_Source}
    ${UpnpRegistry_Header}
    ${UpnpRegistry_Source}
    ${UpnpCtrlPoint_Header}
    ${UpnpCtrlPoint_Source}
    ${UpnpHost_Header}
    ${UpnpHost_Source}
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
#ADD_SUBDIRECTORY(test_ControlPoint)
ADD_SUBDIRECTORY(test_BinaryLight)
