
cmake_minimum_required(VERSION 3.0)

set (CMAKE_CXX_STANDARD 11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
include(cmake/sourceGroupByFolder.cmake)

set(MM_LIB MemoryManagement)
project(${MM_LIB} VERSION 0.1.0)

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
	set( APP_TARGET "macosx" )
	set( APP_MACOSX TRUE )
elseif( CMAKE_SYSTEM_NAME MATCHES "Linux" )
	set( APP_TARGET "linux" )
	set( APP_LINUX TRUE )
elseif( CMAKE_SYSTEM_NAME MATCHES "Windows" )
	set( APP_TARGET "msw" )
	set( APP_MSW TRUE )
else()
	message( FATAL_ERROR "APP_TARGET not defined, and no default for platform '${CMAKE_SYSTEM_NAME}.'" )
endif()

message("-- App is set to ${APP_TARGET}.")

#########################################################################################
#setup testing
set(CATCH_LIB catch)
add_library(${CATCH_LIB} INTERFACE)
set(CATCH_DIR ${CMAKE_SOURCE_DIR}/deps/Catch2 CACHE INTERNAL "Path to Catch installation")
set(CATCH_INCLUDE_DIR ${CATCH_DIR}/single_include CACHE INTERNAL "Path to include folder for Catch")
target_include_directories(${CATCH_LIB} INTERFACE ${CATCH_INCLUDE_DIR})
#include automatic test discovery for ctest
include(${CATCH_DIR}/contrib/Catch.cmake)
#include convenience method for creating tests
include(cmake/createTest.cmake)
enable_testing(true)

#########################################################################################
#setup docs
#doc targets are the projectname prefixed with "doc", this is autmatically generated
#add_custom_target(doc DEPENDS docAProject)

#########################################################################################
#create engine lib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${APP_TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/${APP_TARGET})

file(GLOB ${MM_LIB}_HEADERS
	${CMAKE_SOURCE_DIR}/include/fastsharedptr/*.h
    ${CMAKE_SOURCE_DIR}/include/fastsharedptr/*.hpp
)

include_directories(
	${CMAKE_SOURCE_DIR}/include
)

file(GLOB ${MM_LIB}_SRC
    ${CMAKE_SOURCE_DIR}/src/fastsharedptr/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${${MM_LIB}_SRC} ${${MM_LIB}_HEADERS} )
SOURCE_GROUP_BY_FOLDER(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} 
	PUBLIC ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${ASIO_LIB})
set_target_properties (${PROJECT_NAME} PROPERTIES
    FOLDER /
)

#########################################################################################
#include all tests
#createTest( util-logging-test test/utilities/logging )
#createTest( util-math-test test/utilities/math )
#createTest( util-time-test test/utilities/time )
#createTest( util-threading-test test/utilities/threading )
createTest( fastsharedptr ${CMAKE_SOURCE_DIR}/test/fastsharedptr )









