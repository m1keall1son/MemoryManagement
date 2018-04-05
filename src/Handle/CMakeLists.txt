cmake_minimum_required (VERSION 2.6)
project (handle)
include_directories(src/UnitTest++/src)
add_executable(handle 
	src/Main.cpp 
	src/Handle.h 
	src/HandleManager.h 
	src/HandleManager.cpp
	src/TestHandleManager.cpp
	src/Pow2Assert.h
	src/Pow2Assert.cpp
)