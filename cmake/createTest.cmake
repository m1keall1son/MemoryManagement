cmake_minimum_required(VERSION 3.0)

function(createTest TEST_NAME TEST_DIR)

	message("Creating test: ${TEST_NAME} with locaiton: ${TEST_DIR}")

	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/${TEST_NAME}/${APP_TARGET})

	file(GLOB TEST_FILES
    	${TEST_DIR}/*.cpp
	)

	add_executable(${TEST_NAME} ${TEST_FILES})
	target_include_directories(${TEST_NAME} PUBLIC ${TEST_DIR}/include)
	target_link_libraries(${TEST_NAME} ${CATCH_LIB} ${MM_LIB})

	set_target_properties (${TEST_NAME} PROPERTIES
    	FOLDER tests
    )

	catch_discover_tests(${TEST_NAME})

endFunction(createTest)