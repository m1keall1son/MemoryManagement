
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
add_library(catch INTERFACE)
set(CATCH_DIR ${CMAKE_SOURCE_DIR}/deps/Catch2 CACHE INTERNAL "Path to Catch installation")
set(CATCH_INCLUDE_DIR ${CATCH_DIR}/single_include/catch2 CACHE INTERNAL "Path to include folder for Catch")
target_include_directories(catch INTERFACE ${CATCH_INCLUDE_DIR})
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
file(GLOB allocators_HEADERS
	${CMAKE_SOURCE_DIR}/include/allocators/*.h
    ${CMAKE_SOURCE_DIR}/include/allocators/*.hpp
)
add_library(allocators INTERFACE)
target_sources(allocators INTERFACE ${allocators_HEADERS})
SOURCE_GROUP_BY_FOLDER(allocators)
target_include_directories(allocators INTERFACE ${CMAKE_SOURCE_DIR}/include/allocators)
#set_target_properties (${PROJECT_NAME} PROPERTIES FOLDER allocators)

#########################################################################################
#include all tests
#createTest( util-logging-test test/utilities/logging )
#createTest( util-math-test test/utilities/math )
#createTest( util-time-test test/utilities/time )
#createTest( util-threading-test test/utilities/threading )
createTest(NAME test_allocators LOCATION tests SOURCE ${CMAKE_SOURCE_DIR}/test/allocators LIBS allocators catch)









