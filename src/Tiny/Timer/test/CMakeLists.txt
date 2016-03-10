ADD_EXECUTABLE(test_timer test.c command.c command.h)

TARGET_LINK_LIBRARIES(test_timer sc_timer_static
                                 sc_socket_static
                                 sc_worker_static 
                                 sc_thread_static 
                                 sc_uuid_static 
                                 sc_container_static 
                                 sc_memory_static 
                                 sc_common_static
                                 ${OS_LIB})
