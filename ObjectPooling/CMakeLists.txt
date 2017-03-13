cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake) # main (top) cmake dir
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake) # project specific cmake dir

set(CMAKE_CXX_STANDARD 11) 

project("ObjectPooling")

# binary output
if(APP_MSW)
	if(CMAKE_CL_64)
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64/${PROJECT_NAME}")
		#if making a library uncomment
		#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/x64/${PROJECT_NAME}") 
		#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/x64/${PROJECT_NAME}")
	else()
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x86/${PROJECT_NAME}")
		#if making a library uncomment
		#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/x86/${PROJECT_NAME}") 
		#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/x86/${PROJECT_NAME}")
	endif()
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}")
	#if making a library uncomment
	#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PROJECT_NAME}") 
	#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/${PROJECT_NAME}")
endif()

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

add_subdirectory(src)
add_subdirectory(doc)
add_subdirectory(test)
