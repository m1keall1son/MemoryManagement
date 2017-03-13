cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project("MemoryManagement")

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

if(NOT APP_MSW)
	enable_testing() 
endif()

set(CATCH_INCLUDE_DIR dependencies/catch/single_include CACHE INTERNAL "Path to include folder for Catch")
#setup Catch cpp testing framework
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

#setup docs
#doc targets are the projectname prefixed with "doc", this is autmatically generated
add_custom_target(doc DEPENDS docObjectPooling)

#include all projects
add_subdirectory(ObjectPooling)
