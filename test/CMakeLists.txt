# Locate GTest
find_package(GTest REQUIRED)

# Include GTest headers
include_directories(${GTEST_INCLUDE_DIRS})

# Add test source files
set(TEST_SOURCES
    test_stft.cpp
)

# Create an executable for the tests
add_executable(fastft_tests 
               main.cpp 
               ${TEST_SOURCES})

# Link against GTest and your library
target_link_libraries(fastft_tests 
                      ${GTEST_BOTH_LIBRARIES} 
                      fastft
                      pthread) # Add pthread here

# Optionally, you can copy resources needed for testing to the build directory
# file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})
