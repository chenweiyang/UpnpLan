#-----------------------------------------------------------------------------
#
# Tiny
#
#-----------------------------------------------------------------------------

#-----------------------
# Base
#-----------------------
SET(Base_Header
    Base/tiny_api.h
    Base/tiny_base.h
    Base/tiny_define.h
	Base/tiny_debug.h
    Base/tiny_ret.h
    Base/tiny_time.h
    Base/tiny_typedef.h
    )

SET(Base_Source
    Base/tiny_ret.c
    Base/tiny_time.c
    )

SOURCE_GROUP(TinyBase\\headers              FILES       ${Base_Header})
SOURCE_GROUP(TinyBase\\sources              FILES       ${Base_Source})

#-----------------------
# Log
#-----------------------
SET(Log_Header
    Log/tiny_log.h
    Log/tiny_log_print.h
    )

SET(Log_Source
    Log/tiny_log_print.c
    )

SOURCE_GROUP(TinyLog\\headers               FILES       ${Log_Header})
SOURCE_GROUP(TinyLog\\sources               FILES       ${Log_Source})

#-----------------------
# Memory
#-----------------------
SET(Memory_Header
    Memory/tiny_memory.h
    )

SET(Memory_Source
    Memory/tiny_memory.c
    )

SOURCE_GROUP(TinyMemory\\headers            FILES       ${Memory_Header})
SOURCE_GROUP(TinyMemory\\sources            FILES       ${Memory_Source})

#-----------------------
# Socket
#-----------------------
SET(Socket_Header
    Socket/tiny_socket.h
    Socket/TinySelector.h
    Socket/TinySocketIpc.h
    )

SET(Socket_Source
    Socket/tiny_socket.c
    Socket/TinySelector.c
    Socket/TinySocketIpc.c
    )

SOURCE_GROUP(TinySocket\\headers            FILES       ${Socket_Header})
SOURCE_GROUP(TinySocket\\sources            FILES       ${Socket_Source})

#-----------------------
# Thread
#-----------------------
SET(Thread_Header
    Thread/TinyCondition.h
    Thread/TinyMutex.h
    Thread/TinySemaphore.h
    Thread/TinyThread.h
    )

SET(Thread_Source
    Thread/TinyCondition.c
    Thread/TinyMutex.c
    Thread/TinySemaphore.c
    Thread/TinyThread.c
    )

SOURCE_GROUP(TinyThread\\headers            FILES   ${Thread_Header})
SOURCE_GROUP(TinyThread\\sources            FILES   ${Thread_Source})

#-----------------------
# Timer
#-----------------------
SET(Timer_Header
    Timer/TinyTimer.h
    )

SET(Timer_Source
    Timer/TinyTimer.c
    )

SOURCE_GROUP(TinyTimer\\headers            FILES   ${Timer_Header})
SOURCE_GROUP(TinyTimer\\sources            FILES   ${Timer_Source})

#-----------------------
# Uuid
#-----------------------
SET(Uuid_Header
    Uuid/TinyUuid.h
    )

SET(Uuid_Source
    Uuid/TinyUuid.c
    )

SOURCE_GROUP(TinyUuid\\headers            FILES   ${Uuid_Header})
SOURCE_GROUP(TinyUuid\\sources            FILES   ${Uuid_Source})

#-----------------------
# Container 
#-----------------------
SET(Container_Header
    Container/TinyContainerListener.h
    Container/TinyList.h
    Container/TinyMap.h
    Container/TinyQueue.h)

SET(Container_Source
    Container/TinyList.c
    Container/TinyMap.c
    Container/TinyQueue.c)

SOURCE_GROUP(TinyContainer\\headers         FILES   ${Container_Header})
SOURCE_GROUP(TinyContainer\\sources         FILES   ${Container_Source})

#-----------------------
# Worker
#-----------------------
SET(Worker_Header
    Worker/TinyBlockingQueue.h
    Worker/TinyWorker.h)

SET(Worker_Source
    Worker/TinyBlockingQueue.c
    Worker/TinyWorker.c)

SOURCE_GROUP(TinyWorker\\headers            FILES   ${Worker_Header})
SOURCE_GROUP(TinyWorker\\sources            FILES   ${Worker_Source})

#-----------------------
# Tcp
#-----------------------
SET(Tcp_Header
    Tcp/TcpClient.h
    Tcp/TcpConn.h
    Tcp/TcpConnPool.h
    Tcp/TcpServer.h)

SET(Tcp_Source
    Tcp/TcpClient.c
    Tcp/TcpConn.c
    Tcp/TcpConnPool.c
    Tcp/TcpServer.c)

SOURCE_GROUP(TinyTcp\\headers        FILES     ${Tcp_Header})
SOURCE_GROUP(TinyTcp\\sources        FILES     ${Tcp_Source})

#-----------------------
# String
#-----------------------
SET(String_Header
    String/tiny_char_util.h
    String/tiny_str_equal.h
    String/tiny_str_get_value.h
    String/tiny_str_split.h
    String/tiny_url_split.h)

SET(String_Source
    String/tiny_char_util.c
    String/tiny_str_equal.c
    String/tiny_str_get_value.c
    String/tiny_str_split.c
    String/tiny_url_split.c)

SOURCE_GROUP(TinyString\\headers        FILES     ${String_Header})
SOURCE_GROUP(TinyString\\sources        FILES     ${String_Source})

#-----------------------
# Http
#-----------------------
SET(Http_Header
    Http/AsyncHttpClient.h
    Http/HttpClient.h
    Http/HttpContent.h
    Http/HttpHeader.h
    Http/HttpMessage.h)

SET(Http_Source
    Http/AsyncHttpClient.c
    Http/HttpClient.c
    Http/HttpContent.c
    Http/HttpHeader.c
    Http/HttpMessage.c)

SOURCE_GROUP(TinyHttp\\headers        FILES     ${Http_Header})
SOURCE_GROUP(TinyHttp\\sources        FILES     ${Http_Source})

#-----------------------
# Xml
#-----------------------
SET(Xml_Header
    Xml/TinyXml.h
    Xml/TinyXmlNode.h)

SET(Xml_Source
    Xml/TinyXml.c
    Xml/TinyXmlNode.c)

SOURCE_GROUP(TinyXml\\headers        FILES     ${Xml_Header})
SOURCE_GROUP(TinyXml\\sources        FILES     ${Xml_Source})

#----------------------------------------------------------------------------
# source
#----------------------------------------------------------------------------
SET(SRC
    ${Base_Header}
    ${Base_Source}
    ${Container_Header}
    ${Container_Source}
    ${Http_Header}
    ${Http_Source}
    ${Log_Header}
    ${Log_Source}
    ${Memory_Header}
    ${Memory_Source}
    ${Socket_Header}
    ${Socket_Source}
    ${String_Header}
    ${String_Source}
    ${Tcp_Header}
    ${Tcp_Source}
    ${Thread_Header}
    ${Thread_Source}
    ${Timer_Header}
    ${Timer_Source}
    ${Uuid_Header}
    ${Uuid_Source}
    ${Worker_Header}
    ${Worker_Source}
    ${Xml_Header}
    ${Xml_Source}
    )

#----------------------------------------------------------------------------
# static library
#----------------------------------------------------------------------------
ADD_LIBRARY(tiny_static STATIC ${SRC})
SET_TARGET_PROPERTIES(tiny_static PROPERTIES OUTPUT_NAME "tiny_static")
