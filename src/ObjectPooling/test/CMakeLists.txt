
file(GLOB TEST_FILES
    "*.cpp"
    "../src/*.cpp"
)

include_directories(../src)
set(TESTS_APP_NAME ${PROJECT_NAME}-tests)
add_executable(${TESTS_APP_NAME} ${TEST_FILES})
target_link_libraries(${TESTS_APP_NAME} Catch)

if(NOT APP_MSW)
	enable_testing()
	add_test(NAME ALL_UNIT_TESTS COMMAND ${TESTS_APP_NAME})
endif()