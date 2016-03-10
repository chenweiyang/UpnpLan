#-------------------------------------------------------------------------------------
#
# module: UpnpBase
#
#-------------------------------------------------------------------------------------
SET(UPNP_BASE_SRC     upnp_api.h
                      upnp_debug.h
                      upnp_log.h
                      upnp_log_print.h
                      upnp_macro.h
                      upnp_time.h
                      upnp_log_print.c
                      upnp_time.c)

ADD_LIBRARY(UpnpBase_static STATIC ${UPNP_BASE_SRC})
SET_TARGET_PROPERTIES(UpnpBase_static PROPERTIES LINKER_LANGUAGE CXX)
