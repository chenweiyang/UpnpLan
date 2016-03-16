ADD_EXECUTABLE(test_ctrlpoint test.c command.c command.h)
TARGET_LINK_LIBRARIES(test_ctrlpoint upnp_shared expat ${OS_LIB})