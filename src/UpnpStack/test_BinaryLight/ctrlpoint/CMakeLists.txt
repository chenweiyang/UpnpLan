#-----------------------------------------------------------------------------
#
# BinaryLight - ControlPoint
#
#-----------------------------------------------------------------------------

#-----------------------
# BinaryLight
#-----------------------
SET(BinaryLight_Header
    BinaryLight.h
	SwitchPower.h
	)

SET(BinaryLight_Source
    BinaryLight.c
	SwitchPower.c
	)

SOURCE_GROUP(BinaryLight\\headers        FILES     ${BinaryLight_Header})
SOURCE_GROUP(BinaryLight\\sources        FILES     ${BinaryLight_Source})

#-----------------------
# test
#-----------------------
SET(Test
    test.c
	command.c
	command.h)

#----------------------------------------------------------------------------
# source
#----------------------------------------------------------------------------
SET(SRC
    ${Test}
    ${BinaryLight_Header}
    ${BinaryLight_Source}
    )

ADD_EXECUTABLE(test_binarylight_cp ${SRC})
TARGET_LINK_LIBRARIES(test_binarylight_cp upnp_shared expat ${OS_LIB})
