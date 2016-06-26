cmake_minimum_required(VERSION 3.0.3)
project( sample1 )

set( CMAKE_CXX_FLAGS_RELEASE "-std=c++1y -Wall -Wextra" )
set( CMAKE_CXX_FLAGS_DEBUG "-g -std=c++1y -Wall -Wextra" )
set( CMAKE_BUILD_TYPE Debug )

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

add_executable( main main.cpp lena.cpp )
target_link_libraries( main ${Boost_LIBRARIES} )
target_link_libraries( main crypto )
target_link_libraries( main libsdviz.a )
