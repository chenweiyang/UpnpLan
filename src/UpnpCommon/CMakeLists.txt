#-------------------------------------------------------------------------------------
#
# module: UpnpCommon
#
#-------------------------------------------------------------------------------------
SET(UPNP_COMMON_SRC     DataType.h
                        DataType.cpp)

ADD_LIBRARY(UpnpCommon_static STATIC ${UPNP_COMMON_SRC})
