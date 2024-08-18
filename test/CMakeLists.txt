include(../cmake/FetchDependency.cmake)
include(../cmake/CopySharedToBinaryDir.cmake)
set(FASTFT_TESTS_NAME "${FASTFT_NAME}_tests")

set(gtest_force_shared_crt ON)
# Find or automatically download GTest v1.15.2
FetchDependency(${FASTFT_TESTS_NAME} gtest https://github.com/google/googletest.git v1.15.2)

# Add test source files
set(TEST_SOURCES
    test_stft.cpp
)

# Create an executable for the tests
add_executable(${FASTFT_TESTS_NAME}
               main.cpp 
               ${TEST_SOURCES})
			   
# Include search directories
target_include_directories(${FASTFT_TESTS_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
	${gtest_SOURCE_DIR}/include
)

# Library search directories
#target_link_directories(${FASTFT_TESTS_NAME} PUBLIC
#    $<BUILD_INTERFACE:${gtest_BINARY_DIR}>
#)

# Link against GTest and whatever other dependencies needed by the target
target_link_libraries(${FASTFT_TESTS_NAME}
					  ${PLATFORM_LIBS}
					  gtest gtest_main
                      ${FASTFT_NAME})
add_dependencies(${FASTFT_TESTS_NAME} gtest gtest_main)

CopySharedToBinaryDir(${FASTFT_TESTS_NAME} ${fftw_BINARY_DIR})

# Optionally, you can copy resources needed for testing to the build directory
# file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
