ADD_EXECUTABLE(test_UpnpRegistry test.c command.c command.h)
TARGET_LINK_LIBRARIES(test_UpnpRegistry upnp_static tiny_static expat ${OS_LIB})
