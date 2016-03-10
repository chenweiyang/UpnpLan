ADD_EXECUTABLE(test_socket test.c)

TARGET_LINK_LIBRARIES(test_socket
                                 sc_memory_static 
                                 sc_common_static
                                 ${OS_LIB})
