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
set( EXTERNAL_INCLUDE_DIR "${EXTERNAL_DIR}/include" )

set( json11_DIR "${EXTERNAL_SRC_DIR}/json11" )
ExternalProject_Add( json11
    PREFIX ${EXTERNAL_DIR}
    GIT_REPOSITORY https://github.com/dropbox/json11
    CONFIGURE_COMMAND :
    BUILD_COMMAND g++ -c -std=c++1y ../json11/json11.cpp -o json11.o
    INSTALL_COMMAND install -d ${EXTERNAL_OBJ_DIR} ${EXTERNAL_INCLUDE_DIR} &&  install json11.o ${EXTERNAL_OBJ_DIR}/ && install ${json11_DIR}/json11.hpp ${EXTERNAL_INCLUDE_DIR}/
)

set_source_files_properties(
    ${EXTERNAL_OBJ_DIR}/json11.o PROPERTIES
    EXTERNAL_OBJECT TRUE
    GENERATED TRUE
)

set( SimpleWebServer_DIR "${EXTERNAL_SRC_DIR}/SimpleWebServer" )
ExternalProject_Add( SimpleWebServer
    PREFIX ${EXTERNAL_DIR}
    GIT_REPOSITORY https://github.com/eidheim/Simple-Web-Server
    CONFIGURE_COMMAND :
    BUILD_COMMAND :
    INSTALL_COMMAND install -d ${EXTERNAL_INCLUDE_DIR} && install ${SimpleWebServer_DIR}/server_http.hpp ${SimpleWebServer_DIR}/client_http.hpp ${EXTERNAL_INCLUDE_DIR}/
)

set( SimpleWebSocketServer_DIR "${EXTERNAL_SRC_DIR}/SimpleWebSocketServer" )
ExternalProject_Add( SimpleWebSocketServer
    PREFIX ${EXTERNAL_DIR}
    GIT_REPOSITORY https://github.com/eidheim/Simple-WebSocket-Server
    CONFIGURE_COMMAND :
    BUILD_COMMAND :
    INSTALL_COMMAND install -d ${EXTERNAL_INCLUDE_DIR} && install ${SimpleWebSocketServer_DIR}/server_ws.hpp ${SimpleWebSocketServer_DIR}/client_ws.hpp ${SimpleWebSocketServer_DIR}/crypto.hpp ${EXTERNAL_INCLUDE_DIR}/
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

set( SDVIS_OBJS ${EXTERNAL_OBJ_DIR}/json11.o )

add_library( sdviz STATIC ${SDVIS_SRCS} ${SDVIS_OBJS}  )
INSTALL( TARGETS sdviz ARCHIVE DESTINATION lib )
INSTALL( FILES ${SDVIZ_DIR}/sdviz.hpp DESTINATION include)

add_executable(mytest ${TEST_DIR}/test.cpp)
target_link_libraries( mytest ${Boost_LIBRARIES} )
target_link_libraries( mytest crypto )
target_link_libraries( mytest sdviz )
target_include_directories(mytest PUBLIC ${SDVIZ_DIR})

#add_test(
#    NAME mytest
#    COMMAND $<TARGET_FILE:mytest>
#    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/tmp
#)
