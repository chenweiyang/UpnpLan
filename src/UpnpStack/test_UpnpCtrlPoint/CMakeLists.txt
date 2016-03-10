ADD_EXECUTABLE(test_upnp_ctrlpoint test.c
                                    command.c
								    command.h)
TARGET_LINK_LIBRARIES(test_upnp_ctrlpoint upnp_shared expat)
