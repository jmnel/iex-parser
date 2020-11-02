cmake_minimum_required( VERSION 3.16 FATAL_ERROR )
add_definitions( "-std=c++20" )

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_CXX_EXTENSIONS ON )
set( CMAKE_BUILD_TYPE Debug )

find_program( CCACHE_PROGRAM ccache )
if( CCACHE_PROGRAM )
    set_property( GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache )
    set_property( GLOBAL PROPERTY RULE_LAUNCH_LINK ccache )
endif()

project( iex VERSION 0.1 LANGUAGES CXX )

set(CMAKE_MODULE_PATH ${iex_SOURCE_DIR}/cmake/modules)

find_package( Boost 1.71 REQUIRED COMPONENTS system iostreams )
find_package( PcapPlusPlus 20.08 REQUIRED )
find_package( RdKafka )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(WARNING_FLAGS
    "-pedantic "
    "-Wall "
    "-Werror=return-type "
    "-Wextra "
    "-Wmissing-field-initializers "
    "-Wno-predefined-identifier-outside-function "
    )

set(MISC_FLAGS
    "-fPIC "
#    "-lstdc++fs "
    "-Wpedantic "
    "-fcolor-diagnostics "
#    "-fsanitize=undefined,address "
    "-mlzcnt "
#    "-fsanitize-trap=all "
#    "-fno-exceptions "
#    "-O2 -fprofile-instr-generate -fcoverage-mapping -fprofile-arcs -ftest-coverage "
#    "-O2 "
    )

string(CONCAT WARNING_FLAGS_STR ${WARNING_FLAGS})
string(CONCAT MISC_FLAGS_STR ${MISC_FLAGS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MISC_FLAGS_STR}${WARNING_FLAGS_STR}")


#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${Arcturus_SOURCE_DIR}/../targets/Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/client)
#add_subdirectory( ${CMAKE_CURRENT_LIST_DIR}/libiex )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)

#add_library(iexlib STATIC 
