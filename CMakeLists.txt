cmake_minimum_required(VERSION 3.0.3)
enable_testing()
project( libsdv )
include( ExternalProject )

set( CMAKE_CXX_FLAGS_RELEASE "-std=c++1y -Wall -Wextra" )
set( CMAKE_CXX_FLAGS_DEBUG "-g -std=c++1y -Wall -Wextra" )
#set( CMAKE_BUILD_TYPE Debug )
set( CMAKE_BUILD_TYPE Release )

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

set( Boost_USE_STATIC_LIBS ON )
find_package( Boost
              1.58
              COMPONENTS log
                         log_setup
                         system
                         coroutine
                         context
                         thread
                         regex
              REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

if(APPLE)
    set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(Threads REQUIRED)

set( SDVIZ_DIR "${PROJECT_SOURCE_DIR}/sdviz/" )
set( RESOURCES_DIR "${PROJECT_SOURCE_DIR}/resources/" )
set( TEST_DIR "${PROJECT_SOURCE_DIR}/test/" )
set( EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external/" )
set( EXTERNAL_SRC_DIR "${EXTERNAL_DIR}/src" )
set( EXTERNAL_OBJ_DIR "${EXTERNAL_DIR}/obj" )
set( EXTERNAL_LIB_DIR "${EXTERNAL_DIR}/lib" )
set( EXTERNAL_INCLUDE_DIR "${EXTERNAL_DIR}/include" )

add_custom_target(external_dirs
    COMMAND install -d ${EXTERNAL_DIR}
    COMMAND install -d ${EXTERNAL_SRC_DIR}
    COMMAND install -d ${EXTERNAL_OBJ_DIR}
    COMMAND install -d ${EXTERNAL_LIB_DIR}
    COMMAND install -d ${EXTERNAL_INCLUDE_DIR}
)

set( msgpack11_DIR "${EXTERNAL_SRC_DIR}/msgpack11" )
ExternalProject_Add( msgpack11
    PREFIX ${EXTERNAL_DIR}
    GIT_REPOSITORY https://github.com/ar90n/msgpack11
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTERNAL_DIR}
    DEPENDS external_dirs
)

set( lz4_DIR "${EXTERNAL_SRC_DIR}/lz4" )
ExternalProject_Add( lz4
    PREFIX ${EXTERNAL_DIR}
    GIT_REPOSITORY https://github.com/Cyan4973/lz4
    CONFIGURE_COMMAND :
    BUILD_IN_SOURCE ${EXTERNAL_SRC_DIR}/lz4
    BUILD_COMMAND make -e PREFIX=${EXTERNAL_DIR}
    INSTALL_COMMAND make -e PREFIX=${EXTERNAL_DIR} install && make clean
    DEPENDS external_dirs
)

set( SimpleWebServer_DIR "${EXTERNAL_SRC_DIR}/SimpleWebServer" )
ExternalProject_Add( SimpleWebServer
    PREFIX ${EXTERNAL_DIR}
    GIT_REPOSITORY https://github.com/eidheim/Simple-Web-Server
    CONFIGURE_COMMAND :
    BUILD_COMMAND :
    INSTALL_COMMAND install ${SimpleWebServer_DIR}/server_http.hpp ${SimpleWebServer_DIR}/client_http.hpp ${EXTERNAL_INCLUDE_DIR}/
    DEPENDS external_dirs
)

set( SimpleWebSocketServer_DIR "${EXTERNAL_SRC_DIR}/SimpleWebSocketServer" )
ExternalProject_Add( SimpleWebSocketServer
    PREFIX ${EXTERNAL_DIR}
    GIT_REPOSITORY https://github.com/eidheim/Simple-WebSocket-Server
    CONFIGURE_COMMAND :
    BUILD_COMMAND :
    INSTALL_COMMAND install ${SimpleWebSocketServer_DIR}/server_ws.hpp ${SimpleWebSocketServer_DIR}/client_ws.hpp ${SimpleWebSocketServer_DIR}/crypto.hpp ${EXTERNAL_INCLUDE_DIR}/
    DEPENDS external_dirs
)

include_directories( ${EXTERNAL_INCLUDE_DIR} )

#for debug use
#add_definitions(-DDEBUG_USE_EXTERNAL_TEMPLATE)

set( SDVIS_SRCS ${SDVIZ_DIR}/sdviz.cpp
                ${SDVIZ_DIR}/resource.cpp
                ${SDVIZ_DIR}/context.cpp
                ${SDVIZ_DIR}/image_impl.cpp
                ${SDVIZ_DIR}/canvas_impl.cpp
                ${SDVIZ_DIR}/type_util.cpp
                ${SDVIZ_DIR}/model_sync_server.cpp
                ${SDVIZ_DIR}/serdes.cpp )

add_custom_command(
    OUTPUT ${EXTERNAL_OBJ_DIR}/msgpack11.cpp.o ${EXTERNAL_OBJ_DIR}/lz4.o ${EXTERNAL_OBJ_DIR}/lz4frame.o ${EXTERNAL_OBJ_DIR}/lz4hc.o ${EXTERNAL_OBJ_DIR}/xxhash.o
    COMMAND ${CMAKE_AR} -xv ${EXTERNAL_LIB_DIR}/libmsgpack11.a
    COMMAND ${CMAKE_AR} -xv ${EXTERNAL_LIB_DIR}/liblz4.a
    WORKING_DIRECTORY ${EXTERNAL_OBJ_DIR}
)

add_library( sdviz STATIC ${SDVIS_SRCS}
                          ${EXTERNAL_OBJ_DIR}/msgpack11.cpp.o
                          ${EXTERNAL_OBJ_DIR}/lz4.o
                          ${EXTERNAL_OBJ_DIR}/lz4frame.o
                          ${EXTERNAL_OBJ_DIR}/lz4hc.o
                          ${EXTERNAL_OBJ_DIR}/xxhash.o )
INSTALL( TARGETS sdviz ARCHIVE DESTINATION lib )
INSTALL( FILES ${SDVIZ_DIR}/sdviz.hpp DESTINATION include)

add_dependencies( sdviz SimpleWebSocketServer )
add_dependencies( sdviz SimpleWebServer )
add_dependencies( sdviz msgpack11 )
add_dependencies( sdviz lz4 )
