CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(UpnpLan CXX)

#---------------------------------------------------------------------------------------
# WIN32
#---------------------------------------------------------------------------------------
IF(WIN32)
    MESSAGE(STATUS "build UpnpLan for Win32")
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	
	# unicode for windows
    ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

ELSE(WIN32)
	ADD_DEFINITIONS(-fPIC)

    #-----------------------------------------------------------------------------------
    # APPLE OSX
    #-----------------------------------------------------------------------------------
    IF(APPLE)
        MESSAGE(STATUS "build UpnpLan for osx")
        # Apple LLVM Compiler
        # SET(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++)
        # LLVM gcc Compiler
        SET(CMAKE_CXX_COMPILER /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++)
        SET(CMAKE_CXX_FLAGS -m64)
        #SET(CMAKE_CXX_COMPILER g++)
        #SET(CMAKE_CXX_FLAGS -m64)

    #-----------------------------------------------------------------------------------
    # Linux
    #-----------------------------------------------------------------------------------
    ELSE(APPLE)
    ENDIF(APPLE)
ENDIF(WIN32)

#---------------------------------------------------------------------------------------
#
# include dir
#
#---------------------------------------------------------------------------------------
SET(BOOST_DIR ${PROJECT_SOURCE_DIR}/../boost_1_60_0)
INCLUDE_DIRECTORIES(${BOOST_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/UpnpCommon)

#--------------------------------------------------------------------------------------
#
# 3rdpart library dir
#
#--------------------------------------------------------------------------------------
IF (WIN32)
ELSE (WIN32)
    IF(APPLE)
        SET(LIB_DIR /usr/lib/system/)
    ELSE(APPLE)
    ENDIF(APPLE)
ENDIF (WIN32)

#--------------------------------------------------------------------------------------
#
# lib
#
#--------------------------------------------------------------------------------------
IF (WIN32)
    SET(OS_LIB Iphlpapi.lib)
ELSE (WIN32)
    SET(OS_LIB pthread dl)

    IF(APPLE)
        LINK_DIRECTORIES("/opt/local/lib")
    ELSE(APPLE)
    ENDIF(APPLE)
ENDIF (WIN32)

MESSAGE(STATUS "os_lib : ${OS_LIB}")

#-------------------------------------------------------------------------------------
# set output dir
#-------------------------------------------------------------------------------------
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/output)
MESSAGE(STATUS "executable output:  ${EXECUTABLE_OUTPUT_PATH}")
MESSAGE(STATUS "library output: ${LIBRARY_OUTPUT_PATH}")

#-------------------------------------------------------------------------------------
#
# project
#
#-------------------------------------------------------------------------------------
ADD_SUBDIRECTORY(src)
